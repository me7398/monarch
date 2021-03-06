/*
 * Copyright (c) 2007-2010 Digital Bazaar, Inc. All rights reserved.
 */
#include "monarch/config/ConfigManager.h"

#include <algorithm>
#include <vector>

#include "monarch/config/ConfigChangeListener.h"
#include "monarch/data/json/JsonReader.h"
#include "monarch/data/TemplateInputStream.h"
#include "monarch/io/BufferedOutputStream.h"
#include "monarch/io/ByteArrayInputStream.h"
#include "monarch/io/ByteArrayOutputStream.h"
#include "monarch/io/File.h"
#include "monarch/io/FileList.h"
#include "monarch/io/FileInputStream.h"
#include "monarch/logging/Logging.h"
#include "monarch/util/StringTools.h"

using namespace std;
using namespace monarch::config;
using namespace monarch::data;
using namespace monarch::data::json;
using namespace monarch::io;
using namespace monarch::rt;
using namespace monarch::util;

#define CONFIG_EXCEPTION   "monarch.config.ConfigManager"

const char* ConfigManager::DEFAULT_VALUE = "_default_";
const char* ConfigManager::VERSION       = "_version_";
const char* ConfigManager::ID            = "_id_";
// FIXME: change to GROUPS and have support for multiple groups per config?
// seems like this feature would be simple enough to add without complicating
// too much
const char* ConfigManager::GROUP       = "_group_";
const char* ConfigManager::PARENT      = "_parent_";
const char* ConfigManager::GLOBALS     = "_globals_";
const char* ConfigManager::LOCALS      = "_locals_";
const char* ConfigManager::MERGE       = "_merge_";
const char* ConfigManager::APPEND      = "_append_";
const char* ConfigManager::REMOVE      = "_remove_";
const char* ConfigManager::INCLUDE     = "_include_";
const char* ConfigManager::INCLUDE_EXT = ".config";
const char* ConfigManager::TMP         = "_tmp_";

ConfigManager::ConfigManager() :
   mVersions(Map),
   mKeywordStack(Array),
   mConfigs(Map),
   mConfigChangeListener(NULL)
{
   // initialize internal data structures
   //addVersion(MO_DEFAULT_CONFIG_VERSION);
   // set top of stack to an empty keyword map
   mKeywordStack[0]->setType(Map);
}

ConfigManager::~ConfigManager()
{
   mStates.clear();
}

DynamicObject ConfigManager::getDebugInfo()
{
   DynamicObject debug;

   // read lock to clone configs and versions
   mLock.lockShared();
   {
      debug["configs"] = mConfigs.clone();
      debug["versions"] = mVersions.clone();
   }
   mLock.unlockShared();

   return debug;
}

void ConfigManager::clear()
{
   mLock.lockExclusive();
   mConfigs->clear();
   mLock.unlockExclusive();

   // notify listener of configuration change
   ConfigChangeListener* listener = getConfigChangeListener();
   if(listener != NULL)
   {
      listener->configCleared(this);
   }
}

static void _configChanged(
   ConfigManager* cm, ConfigChangeListener* listener, DynamicObject& changedIds)
{
   // notify listener of all related configuration changes
   DynamicObjectIterator i = changedIds.getIterator();
   while(i->hasNext())
   {
      Config& d = i->next();
      listener->configChanged(cm, i->getName(), d);
   }
}

bool ConfigManager::addConfig(
   Config config, bool include, const char* dir, int level)
{
   bool rval;

   // keep track of changed IDs
   DynamicObject changedIds(Map);
   rval = recursiveAddConfig(config, include, dir, &changedIds, level);

   if(rval)
   {
      ConfigChangeListener* listener = getConfigChangeListener();
      if(listener != NULL)
      {
         // notify listener of configuration addition
         listener->configAdded(this, config[ID]->getString());

         // produce merged diffs, notify listener
         produceMergedDiffs(changedIds);
         _configChanged(this, listener, changedIds);
      }
   }

   return rval;
}

bool ConfigManager::addConfigFile(
   const char* path, bool processIncludes, const char* dir,
   bool optional, bool processSubdirectories, int level)
{
   bool rval = true;

   string userPath;
   // skip empty paths if optional
   // empty paths will behave like current dir
   bool addPath = path != NULL && (strlen(path) != 0 || !optional);

   string levelStr;
   if(level > 0)
   {
      levelStr.assign(level, '*');
      levelStr.push_back(' ');
   }
   
   MO_CAT_DEBUG(MO_CONFIG_CAT,
      "%sAdding config file: \"%s\" from: \"%s\" "
      "flags: %s,%s,%s",
      levelStr.c_str(), path,
      dir != NULL ? dir : "{CURRENT_DIR}",
      processIncludes ? "inc" : "no-inc",
      optional ? "opt" : "not-opt",
      processSubdirectories ? "subdirs" : "no-subdirs");
   if(addPath)
   {
      rval = File::expandUser(path, userPath);
   }

   if(rval && addPath)
   {
      string fullPath;

      // if dir set and expanded user dir not absolute, build a full path
      if(dir && !File::isPathAbsolute(userPath.c_str()))
      {
         fullPath.assign(File::join(dir, userPath.c_str()));
      }
      else
      {
         fullPath.assign(userPath);
      }

      File file(fullPath.c_str());
      if(!file->exists())
      {
         if(!optional)
         {
            ExceptionRef e = new Exception(
               "Configuration file not found.",
               CONFIG_EXCEPTION ".FileNotFound");
            e->getDetails()["path"] = path;
            Exception::set(e);
            rval = false;
         }
      }
      else if(file->isFile())
      {
         // read in configuration
         MO_CAT_DEBUG(MO_CONFIG_CAT,
            "%sLoading config file: \"%s\"",
            levelStr.c_str(), fullPath.c_str());
         FileInputStream is(file);
         JsonReader r;
         Config cfg;
         r.start(cfg);
         rval = r.read(&is) && r.finish();
         is.close();

         if(rval)
         {
            // include path to config (necessary for CURRENT_DIR replacement)
            string dirname = File::dirname(fullPath.c_str());
            rval = addConfig(cfg, processIncludes, dirname.c_str(), level + 1);
         }

         if(!rval)
         {
            ExceptionRef e = new Exception(
               "Configuration file load failure.",
               CONFIG_EXCEPTION ".ConfigFileError");
            e->getDetails()["path"] = path;
            Exception::push(e);
            rval = false;
         }
         MO_CAT_DEBUG(MO_CONFIG_CAT,
            "%sDone loading config file: \"%s\"",
            levelStr.c_str(), fullPath.c_str());
      }
      else if(file->isDirectory())
      {
         MO_CAT_DEBUG(MO_CONFIG_CAT,
            "%sLoading config directory: \"%s\"",
            levelStr.c_str(), fullPath.c_str());
         FileList list;
         file->listFiles(list);

         // find all files with INCLUDE_EXT suffix
         vector<string> configFiles;
         vector<string> configDirs;
         IteratorRef<File> i = list->getIterator();
         while(i->hasNext())
         {
            File& f = i->next();
            string name = f->getAbsolutePath();
            // get basename on full path so "." and ".." are preset.
            // getAbsolutePath has already normalized those paths.
            string basename = File::basename(f->getPath());
            if(f->isFile())
            {
               if(name.rfind(INCLUDE_EXT) ==
                  (name.length() - strlen(INCLUDE_EXT)))
               {
                  configFiles.push_back(basename);
               }
            }
            else if(
               processSubdirectories &&
               strcmp(basename.c_str(), ".") != 0 &&
               strcmp(basename.c_str(), "..") != 0 &&
               f->isDirectory())
            {
               configDirs.push_back(name);
            }
         }

         // sort alphanumerically to allow NN-whatever[.config] ordering
         sort(configFiles.begin(), configFiles.end());
         sort(configDirs.begin(), configDirs.end());

         // load each file in order
         for(vector<string>::iterator i = configFiles.begin();
             rval && i != configFiles.end(); ++i)
         {
            rval = addConfigFile(
               (*i).c_str(), processIncludes, file->getAbsolutePath(), false,
               false, level + 1);
         }

         // load each dir in order
         for(vector<string>::iterator i = configDirs.begin();
             rval && i != configDirs.end(); ++i)
         {
            const char* dir = (*i).c_str();
            rval = addConfigFile(dir, processIncludes, dir, false, false,
               level + 1);
         }
         MO_CAT_DEBUG(MO_CONFIG_CAT,
            "%sDone loading config directory: \"%s\"",
            levelStr.c_str(), fullPath.c_str());
      }
      else
      {
         ExceptionRef e = new Exception(
            "Unknown configuration file type.",
            CONFIG_EXCEPTION ".FileNotFound");
         Exception::set(e);
         rval = false;
      }
   }

   if(!rval)
   {
      ExceptionRef e = new Exception(
         "Invalid config file.",
         CONFIG_EXCEPTION ".InvalidConfigFile");
      e->getDetails()["path"] = path;
      e->getDetails()["processIncludes"] = processIncludes;
      e->getDetails()["optional"] = optional;
      e->getDetails()["processSubdirectories"] = processSubdirectories;
      if(dir != NULL)
      {
         e->getDetails()["dir"] = dir;
      }
      Exception::push(e);
   }

   MO_CAT_DEBUG(MO_CONFIG_CAT,
      "%sDone adding config file: \"%s\"",
      levelStr.c_str(), path);

   return rval;
}

bool ConfigManager::removeConfig(ConfigId id)
{
   bool rval = false;

   // keep track of changed IDs
   DynamicObject changedIds(Map);

   // lock to modify internal storage
   mLock.lockExclusive();
   {
      // FIXME: what happens if a parent or group is removed before
      // its child members are removed? eek! fail or just let the user
      // potentially burn themselves?, easy check is for
      // (mConfigs[id]["children"]->length() != 0)
      if(mConfigs->hasMember(id))
      {
         rval = true;

         // get raw config
         Config& raw = mConfigs[id]["raw"];

         // remove self from parent's children
         if(raw->hasMember(PARENT))
         {
            ConfigId parentId = raw[PARENT]->getString();
            Config& parent = mConfigs[parentId];
            ConfigIterator i = parent["children"].getIterator();
            while(i->hasNext())
            {
               Config& child = i->next();
               if(strcmp(child->getString(), id) == 0)
               {
                  i->remove();
                  break;
               }
            }
         }

         // build list of all related config IDs
         DynamicObject configIds(Array);

         // add group if it has more members
         if(raw->hasMember(GROUP))
         {
            ConfigId groupId = raw[GROUP]->getString();
            Config& group = mConfigs[groupId];
            if(group["members"]->length() > 0)
            {
               // remove member from group
               ConfigIterator i = group["members"].getIterator();
               while(i->hasNext())
               {
                  Config& member = i->next();
                  if(strcmp(member->getString(), id) == 0)
                  {
                     i->remove();
                     break;
                  }
               }

               // group needs update
               configIds->append(raw[GROUP]);
            }
            else
            {
               // remove group, no more members
               mConfigs->removeMember(groupId);
            }
         }

         // add children
         configIds.merge(mConfigs[id]["children"], true);

         // remove config
         mConfigs->removeMember(id);

         // update all related configs
         DynamicObjectIterator i = configIds.getIterator();
         while(i->hasNext())
         {
            update(i->next()->getString(), &changedIds);
         }
      }
      else
      {
         ExceptionRef e = new Exception(
            "Could not remove config. Invalid config ID.",
            CONFIG_EXCEPTION ".InvalidId");
         e->getDetails()["id"] = id;
         Exception::set(e);
      }
   }
   mLock.unlockExclusive();

   if(rval)
   {
      ConfigChangeListener* listener = getConfigChangeListener();
      if(listener != NULL)
      {
         // notify listener of configuration removal
         listener->configRemoved(this, id);

         // produce merged diffs, notify listener
         produceMergedDiffs(changedIds);
         _configChanged(this, listener, changedIds);
      }
   }

   return rval;
}

bool ConfigManager::setConfig(Config config)
{
   bool rval = false;

   // get config ID, store all changed IDs
   ConfigId id = config[ID]->getString();
   DynamicObject changedIds(Map);

   // lock to modify internal storage
   mLock.lockExclusive();
   {
      // ensure the ID exists
      if(!mConfigs->hasMember(id))
      {
         ExceptionRef e = new Exception(
            "Could not set config. Invalid config ID.",
            CONFIG_EXCEPTION ".InvalidId");
         e->getDetails()["id"] = id;
         Exception::set(e);
      }
      // ensure the group ID hasn't changed
      else if(
         (!mConfigs[id]["raw"]->hasMember(GROUP) &&
          config->hasMember(GROUP)) ||
         (mConfigs[id]["raw"]->hasMember(GROUP) &&
          strcmp(config[GROUP]->getString(),
                 mConfigs[id]["raw"][GROUP]->getString()) != 0))
      {
         ExceptionRef e = new Exception(
            "Could not set config. Group changed.",
            CONFIG_EXCEPTION ".ConfigConflict");
         e->getDetails()["id"] = id;
         Exception::set(e);
      }
      // ensure the parent ID hasn't changed
      else if(
         (!mConfigs[id]["raw"]->hasMember(PARENT) &&
          config->hasMember(PARENT)) ||
         (mConfigs[id]["raw"]->hasMember(PARENT) &&
          strcmp(config[PARENT]->getString(),
                 mConfigs[id]["raw"][PARENT]->getString()) != 0))
      {
         ExceptionRef e = new Exception(
            "Could not set config. Parent changed.",
            CONFIG_EXCEPTION ".ConfigConflict");
         e->getDetails()["id"] = id;
         Exception::set(e);
      }
      else
      {
         // only include changed ID if this config's merged config has
         // been generated before (indicating that someone could be
         // watching for changes)
         if(mConfigs[id]->hasMember("merged"))
         {
            changedIds[id] = getMergedConfig(id, false);
         }
         mConfigs[id]["raw"] = config;
         update(id, &changedIds);
         rval = true;
      }
   }
   mLock.unlockExclusive();

   if(rval)
   {
      // notify listener of all configuration changes
      ConfigChangeListener* listener = getConfigChangeListener();
      if(listener != NULL)
      {
         // produce merged diffs, notify listener
         produceMergedDiffs(changedIds);
         _configChanged(this, listener, changedIds);
      }
   }

   return rval;
}

Config ConfigManager::getConfig(ConfigId id, bool raw, bool cache)
{
   Config rval(NULL);

   mLock.lockShared();
   if(mConfigs->hasMember(id))
   {
      /* Note: Returning a reference to the cached config instead of a clone
       * should *not* result in a race condition here. The reason is that
       * the merged config cannot be altered (due to the lock) while we're
       * incrementing its reference count. Any potential race conditions
       * with reference counting in Collectable.h should not be applicable
       * here. Before we release the lock, we will have incremented the
       * reference count on the config so that there should be no fear that
       * the returned config object will be destroyed before the user drops
       * their reference to it. We also do not modify the merged config by
       * making changes to its underlying DynamicObject, rather we replace it
       * entirely if and when a new merged config is generated. This avoids
       * other potential race conditions.
       *
       * Regardless, we still clone the merged config here so that users that
       * accidentally modify it will not interfere with other users that are
       * also using the merged config.
       */
      rval = raw ?
         mConfigs[id]["raw"].clone() :
         getMergedConfig(id, cache).clone();
      mLock.unlockShared();
   }
   else
   {
      mLock.unlockShared();
      ExceptionRef e = new Exception(
         "Could not get config. Invalid config ID.",
         CONFIG_EXCEPTION ".InvalidId");
      e->getDetails()["id"] = id;
      Exception::set(e);
   }

   return rval;
}

bool ConfigManager::hasConfig(ConfigId id)
{
   return mConfigs->hasMember(id);
}

bool ConfigManager::setParent(ConfigId id, ConfigId parentId)
{
   bool rval = false;

   // store all changed IDs
   DynamicObject changedIds(Map);

   // lock to modify internal storage
   mLock.lockExclusive();
   {
      // ensure the ID exists
      if(!mConfigs->hasMember(id))
      {
         ExceptionRef e = new Exception(
            "Could not set config parent ID. Invalid config ID.",
            CONFIG_EXCEPTION ".InvalidId");
         e->getDetails()["id"] = id;
         e->getDetails()["parentId"] = parentId;
         Exception::set(e);
      }
      // ensure the parent exists
      else if(parentId != NULL && !mConfigs->hasMember(parentId))
      {
         ExceptionRef e = new Exception(
            "Could not set config parent ID. Parent ID is invalid.",
            CONFIG_EXCEPTION ".InvalidParent");
         e->getDetails()["id"] = id;
         e->getDetails()["parentId"] = parentId;
         Exception::set(e);
      }
      // don't do any work if the parent is the same as the old one
      else if(
         (parentId == NULL && !mConfigs[id]["raw"]->hasMember(PARENT)) ||
         (parentId != NULL &&
          mConfigs[id]["raw"]->hasMember(PARENT) &&
          strcmp(mConfigs[id]["raw"][PARENT]->getString(), parentId) == 0))
      {
         rval = true;
      }
      else
      {
         // get the IDs of configs that must change
         DynamicObject ids(Array);

         // if config is a member of a group, must change the whole group
         if(mConfigs[id]->hasMember(GROUP))
         {
            id = mConfigs[id][GROUP]->getString();
         }

         // if config is a group then add group ID and all members to ID list
         if(mConfigs[id]->hasMember("members"))
         {
            // change all members and group itself
            ids = mConfigs[id]["members"].clone();
            ids->append(id);
         }
         else
         {
            // just change the single config
            ids->append(id);
         }

         // save the old parent ID
         ConfigId opId = NULL;
         DynamicObject oldParent(NULL);
         if(mConfigs[id]["raw"]->hasMember(PARENT))
         {
            oldParent = mConfigs[id]["raw"][PARENT].clone();
            opId = oldParent;
         }

         // iterate over IDs changing parents
         DynamicObjectIterator i = ids.getIterator();
         while(i->hasNext())
         {
            DynamicObject& next = i->next();
            id = next->getString();

            // only include merged config for changed ID if it has been
            // generated before (indicating that someone could be watching
            // for changes)
            if(mConfigs[id]->hasMember("merged"))
            {
               changedIds[id] = getMergedConfig(id, false);
            }
            if(parentId == NULL)
            {
               // erase parent
               mConfigs[id]["raw"]->removeMember(PARENT);
            }
            else
            {
               // change parent
               mConfigs[id]["raw"][PARENT] = parentId;
               mConfigs[parentId]["children"]->append(id);
            }
         }

         // remove children from old parent
         if(opId != NULL)
         {
            DynamicObjectIterator ci = mConfigs[opId]["children"].getIterator();
            while(ci->hasNext() && ids->length() > 0)
            {
               const char* childId = ci->next();
               i = ids.getIterator();
               while(i->hasNext())
               {
                  if(strcmp(childId, i->next()) == 0)
                  {
                     i->remove();
                     ci->remove();
                     break;
                  }
               }
            }
         }

         // do update
         update(id, &changedIds);
         rval = true;
      }
   }
   mLock.unlockExclusive();

   if(rval)
   {
      // notify listener of all configuration changes
      ConfigChangeListener* listener = getConfigChangeListener();
      if(listener != NULL)
      {
         // produce merged diffs, notify listener
         produceMergedDiffs(changedIds);
         _configChanged(this, listener, changedIds);
      }
   }

   return rval;
}

DynamicObject ConfigManager::getIdsInGroup(ConfigId groupId)
{
   DynamicObject ids;

   mLock.lockShared();
   {
      if(mConfigs->hasMember(groupId) &&
         mConfigs[groupId]->hasMember("members"))
      {
         ids = mConfigs[groupId]["members"].clone();
      }
      else
      {
         ids->setType(Array);
      }
   }
   mLock.unlockShared();

   return ids;
}

void ConfigManager::update(ConfigId id, DynamicObject* changedIds)
{
   // lock to modify internal storage
   mLock.lockExclusive();
   {
      // get config
      Config& config = mConfigs[id];

      if(changedIds != NULL)
      {
         /* Note: Here we only want to report changes for configs that have
          * merged data. If a config does not have any merged data, then its
          * values have not been used yet and so any changes are irrelevant
          * and we can optimize them out. This is particularly useful during
          * initialization of a config system.
          *
          * We also assume here that if the current config we are updating
          * does not have a merged config, then it is necessary that none of
          * its children can have a merged config either (as to generate a
          * child's merged config, the parent merged config must be generated).
          *
          * Therefore, we only save current merged configs for children that
          * have them. We will use these configs later to produce a diff for
          * config change listeners.
          */
         if(config->hasMember("merged"))
         {
            DynamicObjectIterator i = config["children"].getIterator();
            while(i->hasNext())
            {
               ConfigId nextId = i->next()->getString();
               if(!(*changedIds)->hasMember(nextId) &&
                  mConfigs->hasMember(nextId))
               {
                  Config& cfg = mConfigs[nextId];
                  if(cfg->hasMember("merged"))
                  {
                     // save a reference to the old merged config
                     (*changedIds)[nextId] = mConfigs[nextId]["merged"];
                  }
               }
            }
         }
      }

      // if updating a group, recombine members to rebuild RAW config
      if(config->hasMember("members"))
      {
         // clear old raw config
         Config& raw = config["raw"];
         if(raw->hasMember(MERGE))
         {
            raw[MERGE]->clear();
         }
         if(raw->hasMember(APPEND))
         {
            raw[APPEND]->clear();
         }
         if(raw->hasMember(REMOVE))
         {
            raw[REMOVE]->clear();
         }

         // merge raw configs together
         ConfigIterator i = config["members"].getIterator();
         while(i->hasNext())
         {
            Config& memberId = i->next();
            Config& member = mConfigs[memberId->getString()]["raw"];

            // merge the merge property (do not append)
            if(member->hasMember(MERGE))
            {
               merge(raw[MERGE], member[MERGE], false);
            }

            // aggregate append properties
            if(member->hasMember(APPEND))
            {
               merge(raw[APPEND], member[APPEND], true);
            }

            // aggregate remove properties
            if(member->hasMember(REMOVE))
            {
               merge(raw[REMOVE], member[REMOVE], true);
            }
         }
      }

      // wipe old merged config for config ID and replace it with a new one
      // we only do this if there was an existing merged config, otherwise
      // we don't bother as its correct merged config will be lazily created
      // only once a user requests the config
      if(config->hasMember("merged"))
      {
         config->removeMember("merged");
         makeMergedConfig(id, NULL);
      }

      // update group config (if not already the group)
      if(config["raw"]->hasMember(GROUP) &&
         strcmp(id, config["raw"][GROUP]->getString()) != 0)
      {
         update(config["raw"][GROUP]->getString(), changedIds);
      }

      // update each child of config ID
      DynamicObjectIterator i = config["children"].getIterator();
      while(i->hasNext())
      {
         update(i->next()->getString(), changedIds);
      }
   }
   mLock.unlockExclusive();
}

void ConfigManager::setKeyword(const char* keyword, const char* value)
{
   // lock to modify internal storage
   mLock.lockExclusive();
   {
      mKeywordStack.last()[keyword] = value;
   }
   mLock.unlockExclusive();
}

void ConfigManager::addVersion(const char* version)
{
   // lock to modify internal storage
   mLock.lockExclusive();
   {
      mVersions[version] = true;
      MO_CAT_DEBUG(MO_CONFIG_CAT,
         "Added version: \"%s\"", (version != NULL) ? version : "(none)");
   }
   mLock.unlockExclusive();
}

DynamicObject& ConfigManager::getVersions()
{
   return mVersions;
}

void ConfigManager::setConfigChangeListener(ConfigChangeListener* listener)
{
   mLock.lockExclusive();
   mConfigChangeListener = listener;
   mLock.unlockExclusive();
}

ConfigChangeListener* ConfigManager::getConfigChangeListener()
{
   ConfigChangeListener* rval = NULL;

   mLock.lockShared();
   rval = mConfigChangeListener;
   mLock.unlockShared();

   return rval;
}

void ConfigManager::merge(Config& target, Config& source, bool append)
{
   if(source.isNull())
   {
      target = Config(NULL);
   }
   // if the source value is DEFAULT_VALUE then nothing needs to be done to
   // the target to modify it and it can be skipped here
   else if(
      source->getType() != String ||
      strcmp(source->getString(), DEFAULT_VALUE) != 0)
   {
      switch(source->getType())
      {
         case String:
         case Boolean:
         case Int32:
         case UInt32:
         case Int64:
         case UInt64:
         case Double:
            target = source.clone();
            break;
         case Map:
         {
            target->setType(Map);
            ConfigIterator i = source.getIterator();
            while(i->hasNext())
            {
               Config& next = i->next();
               merge(target[i->getName()], next, append);
            }
            break;
         }
         case Array:
         {
            // FIXME: only want to "append" if node is a leaf?
            target->setType(Array);
            int ii = (append ? target->length() : 0);
            ConfigIterator i = source.getIterator();
            for(; i->hasNext(); ++ii)
            {
               merge(target[ii], i->next(), append);
            }
            break;
         }
      }
   }
}

/**
 * A helper method that removes the config values from one config
 * from another config.
 *
 * @param target the target config to update.
 * @param remove the config with entries to remove.
 */
static void _removeLeafNodes(Config& target, Config& remove)
{
   // for each config entry, remove leaf nodes from parent config
   ConfigIterator i = remove.getIterator();
   while(i->hasNext())
   {
      Config& next = i->next();

      // proceed if value is in parent configuration
      if(target->hasMember(i->getName()))
      {
         // FIXME: need a method to remove a single element from an array
         // also -- this currently will not be able to differentiate
         // between removing "index" X and removing value "Y" from an array
         if(!next.isNull() &&
            (next->getType() == Map || next->getType() == Array))
         {
            // empty map/array leaf node to be removed
            if(next->length() == 0)
            {
               target->removeMember(i->getName());
            }
            // recurse to find leaf node
            else
            {
               _removeLeafNodes(target[i->getName()], next);
            }
         }
         else
         {
            // primitive type leaf node to be removed
            target->removeMember(i->getName());
         }
      }
   }
}

void ConfigManager::makeMergedConfig(ConfigId id, Config* out)
{
   // only need to do work if merged config doesn't already exist
   Config& config = mConfigs[id];
   if(!config->hasMember("merged"))
   {
      // produce a merged configuration that contains only config values, not
      // any "_special_" config format values
      Config merged(NULL);

      // get raw configuration
      Config& raw = config["raw"];

      // get merged config from parent
      if(raw->hasMember(PARENT))
      {
         ConfigId parent = raw[PARENT]->getString();
         if(mConfigs[parent]->hasMember("merged"))
         {
            // parent already cached, so just clone it
            merged = mConfigs[parent]["merged"].clone();
         }
         else if(out == NULL)
         {
            // caching is on, so generate and cache parent config
            makeMergedConfig(parent, NULL);
            merged = mConfigs[parent]["merged"].clone();
         }
         else
         {
            // caching is off and parent config not yet cached, so
            // generate parent config and store it in "merged"
            makeMergedConfig(parent, &merged);
         }

         // remove appropriate entries from parent config
         if(raw->hasMember(REMOVE))
         {
            _removeLeafNodes(merged, raw[REMOVE]);
         }

         // merge appropriate entries
         if(raw->hasMember(MERGE))
         {
            merge(merged, raw[MERGE], false);
         }

         // add append field
         if(raw->hasMember(APPEND))
         {
            merge(merged, raw[APPEND], true);
         }
      }
      else
      {
         // clone MERGE field, if it exists
         if(raw->hasMember(MERGE))
         {
            merged = raw[MERGE].clone();

            // add append field, if it exists
            if(raw->hasMember(APPEND))
            {
               merge(merged, raw[APPEND], true);
            }
         }
         // clone APPEND field, if it exists
         else if(raw->hasMember(APPEND))
         {
            merged = raw[APPEND].clone();
         }
         else
         {
            // empty merged config
            merged = Config(Map);
         }
      }

      if(out == NULL)
      {
         // caching is on, set merged config
         config["merged"] = merged;
      }
      else
      {
         // caching is off, return generated config
         *out = merged;
      }
   }
   else if(out != NULL)
   {
      // return cached merged config
      *out = config["merged"];
   }
}

Config ConfigManager::getMergedConfig(ConfigId id, bool cache)
{
   Config rval(NULL);

   if(mConfigs->hasMember(id))
   {
      if(!mConfigs[id]->hasMember("merged"))
      {
         // lazily produce the merged config
         if(cache)
         {
            // generate and cache merged config
            makeMergedConfig(id, NULL);
            rval = mConfigs[id]["merged"];
         }
         else
         {
            // generate merged config but do not cache it
            makeMergedConfig(id, &rval);
         }
      }
      else
      {
         // use cached merged config
         rval = mConfigs[id]["merged"];
      }
   }

   return rval;
}

struct _replaceKeywordsState_s
{
   ByteArrayInputStream* bais;
   TemplateInputStream* tis;
   ByteBuffer* output;
   ByteArrayOutputStream* baos;
};

static bool _replaceKeywords(
   Config& config, DynamicObject& keywordMap,
   struct _replaceKeywordsState_s* state)
{
   bool rval = true;

   if(config.isNull())
   {
      // pass
   }
   else
   {
      switch(config->getType())
      {
         case String:
         {
            // setup template processing chain
            state->bais->setByteArray(config->getString(), config->length());
            state->tis->setVariables(keywordMap, true);
            state->output->clear();
            // reset input stream and parsing state
            state->tis->setInputStream(state->bais);
            rval = state->tis->parse(state->baos);
            if(rval)
            {
               state->output->putByte(0, 1, true);
               // set new string
               config = state->output->data();
            }
            break;
         }
         case Boolean:
         case Int32:
         case UInt32:
         case Int64:
         case UInt64:
         case Double:
            break;
         case Map:
         case Array:
         {
            ConfigIterator i = config.getIterator();
            while(rval && i->hasNext())
            {
               rval = _replaceKeywords(i->next(), keywordMap, state);
            }
            break;
         }
      }
   }

   return rval;
}

bool ConfigManager::replaceKeywords(
   Config& config, DynamicObject& keywordMap, bool full)
{
   bool rval = true;

   if(!config.isNull())
   {
      bool doReplacement = full;
      struct _replaceKeywordsState_s* state = NULL;
      // only process includes and non-meta config info
      static const char* keys[] = {INCLUDE, MERGE, APPEND, REMOVE, NULL};

      if(!full)
      {
         // only create state if needed
         for(int i = 0; !doReplacement && keys[i] != NULL; ++i)
         {
            doReplacement = config->hasMember(keys[i]);
         }
      }
      if(doReplacement)
      {
         // create state if needed
         state = new struct _replaceKeywordsState_s;
         state->bais = new ByteArrayInputStream(NULL, 0);
         state->tis = new TemplateInputStream(state->bais, false);
         state->output = new ByteBuffer(2048);
         state->baos = new ByteArrayOutputStream(state->output, true);

         if(full)
         {
            rval = _replaceKeywords(config, keywordMap, state);
         }
         else
         {
            // replace just certain keywords
            for(int i = 0; rval && keys[i] != NULL; ++i)
            {
               const char* key = keys[i];
               if(config->hasMember(key))
               {
                  rval = _replaceKeywords(config[key], keywordMap, state);
               }
            }
         }

         // clean up state
         delete state->baos;
         delete state->output;
         delete state->tis;
         delete state->bais;
         delete state;
      }
   }

   return rval;
}

bool ConfigManager::diff(
   Config& target, Config& config1, Config& config2, int level,
   DynamicObject* details)
{
   bool rval = false;

   if(config1.isNull() && config2.isNull())
   {
      // same: no diff
   }
   else if(!config1.isNull() && config2.isNull())
   {
      // <stuff> -> NULL: diff=NULL
      rval = true;
      target = Config(NULL);
      if(details != NULL)
      {
         (*details)["message"] = "New config is NULL. Existing config is not.";
      }
   }
   else if(config1.isNull() && !config2.isNull())
   {
      // NULL -> <stuff>: diff=config2
      rval = true;
      target = config2.clone();
      if(details != NULL)
      {
         (*details)["message"] = "Existing config is NULL. New config is not.";
      }
   }
   else if(config1->getType() != config2->getType())
   {
      // types differ: diff=config2
      rval = true;
      target = config2.clone();
      if(details != NULL)
      {
         (*details)["message"] = "Config types are not the same.";
         (*details)["existing"] =
            DynamicObject::descriptionForType(config1->getType());
         (*details)["new"] =
            DynamicObject::descriptionForType(config2->getType());
      }
   }
   else
   {
      // not null && same type: diff=deep compare
      switch(config1->getType())
      {
         case String:
         case Boolean:
         case Int32:
         case UInt32:
         case Int64:
         case UInt64:
         case Double:
            // compare simple types directly
            if(config1 != config2)
            {
               // changed: diff=config2
               rval = true;
               target = config2.clone();
               if(details != NULL)
               {
                  (*details)["message"] =
                     "Existing primitive type value does not match.";
                  (*details)["existing"] = config1.clone();
                  (*details)["new"] = config2.clone();
               }
            }
            break;
         case Map:
         {
            // compare config2 keys since we are only concerned with
            // additions and updates, not removals
            ConfigIterator i = config2.getIterator();
            while(i->hasNext())
            {
               Config next = i->next();
               const char* name = i->getName();
               if(strcmp(name, TMP) != 0)
               {
                  // ignore ID, APPEND, and REMOVE properties
                  if(level != 0 ||
                     (strcmp(name, ID) != 0 &&
                      strcmp(name, APPEND) != 0 &&
                      strcmp(name, REMOVE) != 0))
                  {
                     if(!config1->hasMember(name))
                     {
                        // ensure VERSION, PARENT, and GROUP exist in both
                        if(level == 0 &&
                           (strcmp(name, VERSION) == 0 ||
                            strcmp(name, PARENT) == 0 ||
                            strcmp(name, GROUP) == 0))
                        {
                           // special property not in config1, so add to diff
                           rval = true;
                           target[name] = next.clone();
                           if(details != NULL)
                           {
                              (*details)["message"] =
                                 "A version, parent, or group exists in the "
                                 "new config but doesn't in the existing "
                                 "config.";
                              (*details)["conflict"] = name;
                              (*details)["existing"] = config1;
                           }
                        }
                     }
                     else
                     {
                        // recusively get sub-diff
                        Config d;
                        if(details != NULL)
                        {
                           DynamicObject sd;
                           if(diff(d, config1[name], next, level + 1, &sd))
                           {
                              // diff found, add it
                              rval = true;
                              target[name] = d;
                              (*details)[name] = sd;
                           }
                        }
                        else if(diff(d, config1[name], next, level + 1))
                        {
                           // diff found, add it
                           rval = true;
                           target[name] = d;
                        }
                     }
                  }
               }
            }
            break;
         }
         case Array:
         {
            // compare config2 indexes since we are only concerned with
            // additions and updates, not removals
            Config temp(Array);
            if(details != NULL)
            {
               (*details)->setType(Array);
            }
            ConfigIterator i = config2.getIterator();
            for(int ii = 0; i->hasNext(); ++ii)
            {
               DynamicObject next = i->next();
               Config d;
               if(details != NULL)
               {
                  DynamicObject sd;
                  if(diff(d, config1[ii], next, level + 1, &sd))
                  {
                     // diff found
                     rval = true;
                     temp[ii] = d;
                     (*details)[ii] = sd;
                  }
                  else
                  {
                     // set keyword value
                     temp[ii] = DEFAULT_VALUE;
                     (*details)[ii]->setType(Map);
                  }
               }
               else if(diff(d, config1[ii], next, level + 1))
               {
                  // diff found
                  rval = true;
                  temp[ii] = d;
               }
               else
               {
                  // set keyword value
                  temp[ii] = DEFAULT_VALUE;
               }
            }

            // only set array to target if a diff was found
            if(rval)
            {
               target = temp;
            }

            break;
         }
      }
   }

   return rval;
}

bool ConfigManager::checkConflicts(
   ConfigId id, Config& existing, Config& config, bool isGroup)
{
   bool rval = true;

   // calculate the conflict-diff between existing and config
   Config d;
   DynamicObject details;
   diff(d, existing, config, 0, &details);

   // check for parent, group, or merge conflicts
   // version check done elsewhere
   if(d->hasMember(PARENT) ||
      d->hasMember(GROUP) ||
      d->hasMember(MERGE))
   {
      ExceptionRef e = new Exception(
         "Config conflict. Parent, group, or merge field differs for "
         "a particular config ID.",
         CONFIG_EXCEPTION ".ConfigConflict");
      e->getDetails()["configId"] = id;
      e->getDetails()["diff"] = d;
      e->getDetails()["isGroup"] = isGroup;
      e->getDetails()["details"] = details;
      Exception::set(e);
      rval = false;
   }

   return rval;
}

void ConfigManager::produceMergedDiffs(DynamicObject& changedIds)
{
   // lock while producing merged diffs
   mLock.lockExclusive();
   {
      DynamicObject tmp = changedIds;
      changedIds = DynamicObject(Map);
      DynamicObjectIterator i = tmp.getIterator();
      while(i->hasNext())
      {
         Config& oldMerged = i->next();
         ConfigManager::ConfigId nextId = i->getName();
         if(mConfigs->hasMember(nextId))
         {
            // diff new merged config with the old one we saved a copy of
            Config d;
            Config newMerged = getMergedConfig(nextId, true);
            if(diff(d, oldMerged, newMerged))
            {
               changedIds[nextId] = d;
            }
         }
      }
   }
   mLock.unlockExclusive();
}

/**
 * A helper method to insert a config. This method assumes there is no
 * existing config with the passed ID and that any parent in the config is
 * valid.
 *
 * @param id the config ID of the config to insert.
 * @param storage the config to use for storage.
 * @param raw the raw config to insert.
 */
static void _insertConfig(
   ConfigManager::ConfigId id, Config& storage, Config& raw)
{
   Config& c = storage[id];
   c["children"]->setType(Array);
   c["raw"] = raw;

   // if has parent
   if(raw->hasMember(ConfigManager::PARENT))
   {
      // update parent's children
      ConfigManager::ConfigId parent = raw[ConfigManager::PARENT]->getString();
      storage[parent]["children"]->append(id);
   }
}

bool ConfigManager::recursiveAddConfig(
   Config& config, bool include, const char* dir, DynamicObject* changedIds,
   int level)
{
   bool rval = true;

   // get config ID
   ConfigId id = "";
   if(!config.isNull() && config->hasMember(ID))
   {
      id = config[ID]->getString();
   }
   else
   {
      ExceptionRef e = new Exception(
         "No valid config ID found.",
         CONFIG_EXCEPTION ".MissingId");
      e->getDetails()["config"] = config;
      Exception::set(e);
      rval = false;
   }

   // ensure group ID doesn't match config ID
   if(rval && config->hasMember(GROUP) &&
      strcmp(id, config[GROUP]->getString()) == 0)
   {
      ExceptionRef e = new Exception(
         "Group ID cannot be the same as config ID.",
         CONFIG_EXCEPTION ".ConfigConflict");
      e->getDetails()["id"] = id;
      Exception::set(e);
      rval = false;
   }

   if(rval)
   {
      // lock to check version & parent
      mLock.lockExclusive();
      {
         // if config has version, check it is supported
         if(config->hasMember(VERSION))
         {
            // check for known version
            const char* version = config[VERSION]->getString();
            if(!mVersions->hasMember(version))
            {
               ExceptionRef e = new Exception(
                  "Unsupported config file version.",
                  CONFIG_EXCEPTION ".UnsupportedVersion");
               e->getDetails()["id"] = id;
               e->getDetails()["version"] = version;
               Exception::set(e);
               rval = false;
            }
         }
         /*
         else if(mStrict)
         {
            ExceptionRef e = new Exception(
               "No config file version found.",
               CONFIG_EXCEPTION ".UnspecifiedVersion");
            e->getDetails()["id"] = id;
            Exception::set(e);
            rval = false;
         }
         */

         // if has parent
         if(rval && config->hasMember(PARENT))
         {
            // ensure parent exists
            ConfigId parent = config[PARENT]->getString();
            if(!mConfigs->hasMember(parent))
            {
               // FIXME: Allow configs to be added with invalid parents when in
               // non-strict mode. They will be silently created as blank
               // configs?
               if(true)//mStrict)
               {
                  ExceptionRef e = new Exception(
                     "Invalid parent configuration file ID.",
                     CONFIG_EXCEPTION ".InvalidParent");
                  e->getDetails()["configId"] = id;
                  e->getDetails()["parentId"] = parent;
                  Exception::set(e);
                  rval = false;
               }
               else
               {
                  // FIXME: need a flag to prevent conflicts since a parent
                  // hasn't actually been added yet
                  // create empty parent
                  //mConfigs[parent][ID] = parent;
               }
            }
         }
      }
      mLock.unlockExclusive();
   }

   // add global keywords to entire keyword stack
   if(rval)
   {
      if(config->hasMember(GLOBALS))
      {
         mLock.lockExclusive();
         DynamicObjectIterator i = mKeywordStack.getIterator();
         while(i->hasNext())
         {
            DynamicObject& state = i->next();
            state.merge(config[GLOBALS], false);
         }
         mLock.unlockExclusive();
      }
   }

   // push a new keyword state onto the stack and merge current keywords
   if(rval)
   {
      mLock.lockExclusive();
      DynamicObject state = mKeywordStack.last().clone();
      if(config->hasMember(LOCALS))
      {
         state.merge(config[LOCALS], false);
      }
      mKeywordStack.push(state);
      mLock.unlockExclusive();
   }

   // handle global keyword replacement
   if(rval)
   {
      // add special current directory keyword
      if(dir != NULL)
      {
         mLock.lockExclusive();
         mKeywordStack.last()["CURRENT_DIR"] = dir;
         mLock.unlockExclusive();
      }

      // do keyword replacement (custom and special)
      mLock.lockShared();
      DynamicObject kw = mKeywordStack.last();
      rval = replaceKeywords(config, kw);
      mLock.unlockShared();

      // remove special keywords
      if(dir != NULL)
      {
         mLock.lockExclusive();
         mKeywordStack.last()->removeMember("CURRENT_DIR");
         mLock.unlockExclusive();
      }
   }

   // process includes
   if(rval && include && config->hasMember(INCLUDE))
   {
      if(config[INCLUDE]->getType() != Array)
      {
         ExceptionRef e = new Exception(
            "The include directive value must be an array.",
            CONFIG_EXCEPTION ".InvalidIncludeType");
         e->getDetails()["configId"] = id;
         e->getDetails()[INCLUDE] = config[INCLUDE];
         Exception::set(e);
         rval = false;
      }
      else
      {
         ConfigIterator i = config[INCLUDE].getIterator();
         while(rval && i->hasNext())
         {
            Config& next = i->next();
            bool load = true;
            bool optional = false;
            bool includeSubdirectories = false;
            const char* path = NULL;

            if(next->getType() == String)
            {
               path = next->getString();
            }
            else if(next->getType() == Map)
            {
               if(next->hasMember("path"))
               {
                  path = next["path"]->getString();
               }
               else
               {
                  ExceptionRef e = new Exception(
                     "The include path is missing.",
                     CONFIG_EXCEPTION ".MissingIncludePath");
                  e->getDetails()["configId"] = id;
                  e->getDetails()[INCLUDE] = config[INCLUDE];
                  Exception::set(e);
                  rval = false;
               }
               // should include be loaded?
               if(next->hasMember("load"))
               {
                  load = next["load"]->getBoolean();;
               }
               // is include optional?
               if(next->hasMember("optional"))
               {
                  optional = next["optional"]->getBoolean();
               }
               // should subdirs be scanned too?
               if(next->hasMember("includeSubdirectories"))
               {
                  includeSubdirectories =
                     next["includeSubdirectories"]->getBoolean();
               }
            }
            else
            {
               ExceptionRef e = new Exception(
                  "The type of the include value is invalid.",
                  CONFIG_EXCEPTION ".InvalidIncludeType");
               e->getDetails()["configId"] = id;
               e->getDetails()[INCLUDE] = config[INCLUDE];
               Exception::set(e);
               rval = false;
            }

            // load is set to do so
            if(rval && load)
            {
               rval = addConfigFile(
                  path, true, dir, optional, includeSubdirectories, level + 1);
            }
         }
      }
   }

   // add configuration
   if(rval)
   {
      // lock to add config to internal storage
      mLock.lockExclusive();
      {
         // get the group ID
         ConfigId groupId = "";
         bool group = false;
         if(config->hasMember(GROUP))
         {
            group = true;
            groupId = config[GROUP]->getString();
         }

         // if the config ID already exists, ensure there are no conflicts
         bool mergeConfig = false;
         if(mConfigs->hasMember(id))
         {
            mergeConfig = true;
            rval = checkConflicts(id, mConfigs[id]["raw"], config, false);
         }

         // if the group ID already exists, ensure there are no conflicts
         if(rval && group && mConfigs->hasMember(groupId))
         {
            rval = checkConflicts(
               groupId, mConfigs[groupId]["raw"], config, true);
         }

         if(rval)
         {
            if(mergeConfig)
            {
               Config& raw = mConfigs[id]["raw"];

               // merge the merge property (do not append)
               if(config->hasMember(MERGE))
               {
                  merge(raw[MERGE], config[MERGE], false);
               }

               // aggregate append properties
               if(config->hasMember(APPEND))
               {
                  merge(raw[APPEND], config[APPEND], true);
               }

               // aggregate remove properties
               if(config->hasMember(REMOVE))
               {
                  merge(raw[REMOVE], config[REMOVE], true);
               }
            }
            else
            {
               // insert config
               _insertConfig(id, mConfigs, config);
            }

            if(group)
            {
               // add group if it does not exist
               if(!mConfigs->hasMember(groupId))
               {
                  // insert blank group config, will be updated via update()
                  // Note: Only implicit groups can have the same group ID
                  // and config ID ... as their "raw" config is fake ... and
                  // cannot be retrieved outside of the config manager.
                  Config& groupConfig = mConfigs[groupId];
                  groupConfig["raw"][ID] = groupId;
                  groupConfig["raw"][GROUP] = groupId;
                  groupConfig["children"]->setType(Array);
                  if(config->hasMember(PARENT))
                  {
                     groupConfig["raw"][PARENT] = config[PARENT]->getString();
                  }
                  groupConfig["members"]->append(id);
               }
               // add member to group if not already in group
               else
               {
                  bool add = true;
                  Config& groupConfig = mConfigs[groupId];
                  ConfigIterator i = groupConfig["members"].getIterator();
                  while(add && i->hasNext())
                  {
                     Config& member = i->next();
                     if(strcmp(member->getString(), id) == 0)
                     {
                        add = false;
                     }
                  }
                  if(add)
                  {
                     groupConfig["members"]->append(id);
                  }
               }
            }
         }

         if(rval)
         {
            // only update related merged configs
            update(id, changedIds);
         }
      }
      mLock.unlockExclusive();
   }

   // pop keyword state from the stack
   if(rval)
   {
      mLock.lockExclusive();
      mKeywordStack.pop();
      mLock.unlockExclusive();
   }

   return rval;
}

void ConfigManager::saveState()
{
   mLock.lockExclusive();
   {
      DynamicObject state;
      state["versions"] = mVersions.clone();
      state["keywords"] = mKeywordStack.clone();
      state["configs"] = mConfigs.clone();
      mStates.push_back(state);
   }
   mLock.unlockExclusive();
}

bool ConfigManager::restoreState()
{
   bool rval = true;

   mLock.lockExclusive();
   {
      if(mStates.size() == 0)
      {
         ExceptionRef e = new Exception(
            "Could not restore ConfigManager state. No previously saved state "
            "found.",
            CONFIG_EXCEPTION ".SavedStateNotFound");
         Exception::set(e);
         rval = false;
      }
      else
      {
         DynamicObject state = mStates.back();
         mStates.pop_back();
         mVersions = state["versions"];
         mKeywordStack = state["keywords"];
         mConfigs = state["configs"];
      }
   }
   mLock.unlockExclusive();

   return rval;
}
