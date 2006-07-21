/*
 * Copyright (c) 2006 Digital Bazaar, Inc.  All rights reserved.
 */
package com.db.autoupdater;

import java.net.URL;
import java.net.URLClassLoader;

import com.db.event.EventDelegate;
import com.db.event.EventObject;
import com.db.logging.Logger;
import com.db.logging.LoggerManager;
import com.db.util.ConfigOptions;
import com.db.util.MethodInvoker;

/**
 * An AbstractAutoUpdater is a basic implementation of an AutoUpdater. It is
 * an application automatic updater. It retrieves an update script from a
 * UpdateScriptSource and processes it to update an application.
 * 
 * @author Dave Longley
 */
public abstract class AbstractAutoUpdater implements AutoUpdater
{
   /**
    * Set to true when this AutoUpdater requires a reload, false otherwise.
    */
   protected boolean mRequiresReload;
   
   /**
    * True while processing an update, false otherwise. 
    */
   protected boolean mProcessingUpdate;
   
   /**
    * True when this AutoUpdater should automatically check for updates, false
    * when it should not.
    */
   protected boolean mAutoCheckForUpdate;
   
   /**
    * The number of milliseconds to sleep in between automatic update checks.
    */
   protected long mAutoCheckForUpdateInterval;
   
   /**
    * A reference to the auto update checker thread.
    */
   protected Thread mAutoCheckThread;
   
   /**
    * An event delegate for check for update started events.
    */
   protected EventDelegate mCheckForUpdateStartedEventDelegate;
   
   /**
    * An event delegate for update script found events.
    */
   protected EventDelegate mUpdateScriptFoundEventDelegate;
   
   /**
    * An event delegate for update script not found events.
    */
   protected EventDelegate mUpdateScriptNotFoundEventDelegate;

   /**
    * An event delegate for update script completed events.
    */
   protected EventDelegate mUpdateScriptCompletedEventDelegate;

   /**
    * An event delegate for update script cancelled events.
    */
   protected EventDelegate mUpdateScriptCancelledEventDelegate;
   
   /**
    * An event delegate for update script failed events.
    */
   protected EventDelegate mUpdateScriptFailedEventDelegate;
   
   /**
    * An event delegate for update script reverted events.
    */
   protected EventDelegate mUpdateScriptRevertedEventDelegate;
   
   /**
    * An event delegate for update script revert failed events.
    */
   protected EventDelegate mUpdateScriptRevertFailedEventDelegate;
   
   /**
    * An event delegate for update script processed events.
    */
   protected EventDelegate mUpdateScriptProcessedEventDelegate;

   /**
    * An event delegate for execute application events.
    */
   protected EventDelegate mExecuteApplicationEventDelegate;
   
   /**
    * An event delegate for shutdown application events.
    */
   protected EventDelegate mShutdownApplicationEventDelegate;

   /**
    * An event delegate for application shutdown events.
    */
   protected EventDelegate mApplicationShutdownEventDelegate;
   
   /**
    * Creates a new AutoUpdater.
    */
   public AbstractAutoUpdater()
   {
      // no reload required by default
      setRequiresReload(false);
      
      // not processing an update by default
      setProcessingUpdate(false);
      
      // do not auto check for updates by default
      setAutoCheckForUpdate(false);
      
      // default the auto check interval to 30 seconds
      setAutoCheckForUpdateInterval(30000);
      
      // create check for update started event delegate
      mCheckForUpdateStartedEventDelegate = new EventDelegate();

      // create update script found event delegate
      mUpdateScriptFoundEventDelegate = new EventDelegate();
      
      // create update script not found event delegate
      mUpdateScriptNotFoundEventDelegate = new EventDelegate();
      
      // create update script completed delegate
      mUpdateScriptCompletedEventDelegate = new EventDelegate();
      
      // create update script cancelled delegate
      mUpdateScriptCancelledEventDelegate = new EventDelegate();

      // create update script failed delegate
      mUpdateScriptFailedEventDelegate = new EventDelegate();
      
      // create update script reverted delegate
      mUpdateScriptRevertedEventDelegate = new EventDelegate();
      
      // create update script revert failed delegate
      mUpdateScriptRevertFailedEventDelegate = new EventDelegate();
      
      // create update script processed delegate
      mUpdateScriptProcessedEventDelegate = new EventDelegate();
      
      // create execute application event delegate
      mExecuteApplicationEventDelegate = new EventDelegate();
      
      // create shutdown application event delegate
      mShutdownApplicationEventDelegate = new EventDelegate();
      
      // create application shutdown delegate
      mApplicationShutdownEventDelegate = new EventDelegate();
   }
   
   /**
    * Fires a check for update started event.
    * 
    * @param event the event to fire.
    */
   protected void fireCheckForUpdateStartedEvent(EventObject event)
   {
      mCheckForUpdateStartedEventDelegate.fireEvent(event);
   }

   /**
    * Fires an update script found event.
    * 
    * @param event the event to fire.
    */
   protected void fireUpdateScriptFoundEvent(EventObject event)
   {
      mUpdateScriptFoundEventDelegate.fireEvent(event);
   }
   
   /**
    * Fires an update script not found event.
    * 
    * @param event the event to fire.
    */
   protected void fireUpdateScriptNotFoundEvent(EventObject event)
   {
      mUpdateScriptNotFoundEventDelegate.fireEvent(event);
   }
   
   /**
    * Fires an update script completed event.
    * 
    * @param event the event to fire.
    */
   protected void fireUpdateScriptCompletedEvent(EventObject event)
   {
      mUpdateScriptCompletedEventDelegate.fireEvent(event);
   }
   
   /**
    * Fires an update script cancelled event.
    * 
    * @param event the event to fire.
    */
   protected void fireUpdateScriptCancelledEvent(EventObject event)
   {
      mUpdateScriptCancelledEventDelegate.fireEvent(event);
   }
   
   /**
    * Fires an update script failed event.
    * 
    * @param event the event to fire.
    */
   protected void fireUpdateScriptFailedEvent(EventObject event)
   {
      mUpdateScriptFailedEventDelegate.fireEvent(event);
   }
   
   /**
    * Fires an update script reverted event.
    * 
    * @param event the event to fire.
    */
   protected void fireUpdateScriptRevertedEvent(EventObject event)
   {
      mUpdateScriptRevertedEventDelegate.fireEvent(event);
   }
   
   /**
    * Fires an update script revert failed event.
    * 
    * @param event the event to fire.
    */
   protected void fireUpdateScriptRevertFailedEvent(EventObject event)
   {
      mUpdateScriptRevertFailedEventDelegate.fireEvent(event);
   }
   
   /**
    * Fires an update script processed event.
    * 
    * @param event the event to fire.
    */
   protected void fireUpdateScriptProcessedEvent(EventObject event)
   {
      mUpdateScriptProcessedEventDelegate.fireEvent(event);
   }

   /**
    * Fires an execute application event.
    * 
    * @param event the event to fire.
    */
   protected void fireExecuteApplicationEvent(EventObject event)
   {
      mExecuteApplicationEventDelegate.fireEvent(event);
   }
   
   /**
    * Fires a shutdown application event.
    * 
    * @param event the event to fire.
    */
   protected void fireShutdownApplicationEvent(EventObject event)
   {
      mShutdownApplicationEventDelegate.fireEvent(event);
   }

   /**
    * Fires an application shutdown event.
    * 
    * @param event the event to fire.
    */
   protected void fireApplicationShutdownEvent(EventObject event)
   {
      mApplicationShutdownEventDelegate.fireEvent(event);
   }

   /**
    * Sets whether or not this AutoUpdater requires a reload.
    * 
    * @param reload true if this AutoUpdater requires a reload, false if not.
    */
   protected synchronized void setRequiresReload(boolean reload)
   {
      mRequiresReload = reload;
   }
   
   /**
    * Sets whether or not this AutoUpdater is processing an update.
    * 
    * @param processing true if this AutoUpdater is processing an update,
    *                   false if not.
    */
   protected synchronized void setProcessingUpdate(boolean processing)
   {
      mProcessingUpdate = processing;
   }
   
   /**
    * Sets whether or not this AutoUpdater should automatically check
    * for updates.
    * 
    * @param check true if this AutoUpdater should automatically check for
    *              updates, false if not.
    */
   protected void setAutoCheckForUpdate(boolean check)
   {
      mAutoCheckForUpdate = check;
   }
   
   /**
    * Gets whether or not this AutoUpdater should automatically check
    * for updates.
    * 
    * @return true if this AutoUpdater should automatically check for
    *         updates, false if not.
    */
   protected boolean shouldAutoCheckForUpdate()
   {
      return mAutoCheckForUpdate;
   }
   
   /**
    * Checks for an update for the given application.
    * 
    * @param application the application to check for updates of.
    * 
    * @return true if an update was processed, false if not.
    */
   protected synchronized boolean checkForUpdate(AutoUpdateable application)
   {
      boolean rval = false;
      
      // fire a check for update started event
      EventObject event = new EventObject("checkForUpdateStarted");
      fireCheckForUpdateStartedEvent(event);
      
      // get the update script source
      UpdateScriptSource source = getUpdateScriptSource();
      
      // check for an update script
      if(source.hasUpdateScript(application))
      {
         // get the update script
         UpdateScript script = source.getUpdateScript(application);
         
         // validate the script
         if(script.validate())
         {
            // fire event indicating that an update script has been found
            event = new EventObject("updateScriptFound");
            event.setData("updateScript", script);
            event.setDataKeyMessage("updateScript",
               "The update script, class=" + script.getClass().getName());
            event.setData("processUpdate", false);
            event.setDataKeyMessage("processUpdate",
               "A boolean if set to true tells the AutoUpdater to " +
               "process the update script.");
            fireUpdateScriptFoundEvent(event);
            
            // see if the update should be processed
            if(event.getDataBooleanValue("processUpdate"))
            {
               // process update script
               rval = true;
               processUpdateScript(application, script);
            }
         }
         else
         {
            // fire event indicating that an update script has not been found
            event = new EventObject("updateScriptNotFound");
            fireUpdateScriptNotFoundEvent(event);
         }
      }
      else
      {
         // fire event indicating that an update script has not been found
         event = new EventObject("updateScriptNotFound");
         fireUpdateScriptNotFoundEvent(event);
      }
      
      return rval;
   }
   
   /**
    * Shutsdown any running application and processes an update script.
    * 
    * @param application the application that is running.
    * @param script the update script to process.
    */
   protected synchronized void processUpdateScript(
      AutoUpdateable application, UpdateScript script)
   {
      // ensure this thread is not interrupted
      if(!Thread.currentThread().isInterrupted())
      {
         // now processing an update
         setProcessingUpdate(true);
         
         // fire event indicating the auto-updateable application is
         // getting shutdown
         EventObject event = new EventObject("shutdownApplication");
         event.setData("cancel", false);
         event.setDataKeyMessage("cancel",
            "A boolean if set to true cancels application shutdown.");
         fireShutdownApplicationEvent(event);
         
         // make sure shutdown was not cancelled
         if(!event.getDataBooleanValue("cancel"))
         {
            // shutdown the application
            application.shutdown();
            
            // fire event indicating the auto-updateable application has been
            // shutdown
            event = new EventObject("applicationShutdown");
            fireApplicationShutdownEvent(event);
            
            // process the script
            boolean success = script.process();

            // set whether or not this AutoUpdater requires a reload
            setRequiresReload(script.autoUpdaterRequiresReload());

            if(success)
            {
               // script processing was successful
               
               // fire event indicating an update script was completed
               event = new EventObject("updateScriptCompleted");
               event.setData("updateScript", script);
               event.setDataKeyMessage("updateScript",
                     "The update script, class=" + script.getClass().getName());
               fireUpdateScriptCompletedEvent(event);
            }
            else
            {
               // script processing was cancelled or there was an error

               // create a new event to fire
               event = new EventObject();
               event.setData("updateScript", script);
               event.setDataKeyMessage("updateScript",
                  "The update script, class=" + script.getClass().getName());
               event.setData("revert", true);
               event.setDataKeyMessage("revert",
                  "A boolean if set true will tell the AutoUpdater to" +
                  "revert the update script.");
               
               // fire event indicating an update script was cancelled or
               // failed and will now be reverted unless cancelled
               if(script.cancelled())
               {
                  event.setName("updateScriptCancelled");
                  fireUpdateScriptCancelledEvent(event);
               }
               else
               {
                  event.setName("updateScriptFailed");
                  fireUpdateScriptFailedEvent(event);
               }

               if(event.getDataBooleanValue("revert"))
               {
                  // attempt to revert script
                  if(script.revert())
                  {
                     // fire event indicating an update script was reverted
                     event = new EventObject("updateScriptReverted");
                     event.setData("updateScript", script);
                     event.setDataKeyMessage("updateScript",
                        "The update script, class=" +
                        script.getClass().getName());
                     fireUpdateScriptRevertedEvent(event);
                  }
                  else
                  {
                     // fire event indicating an update script revert failed
                     event = new EventObject("updateScriptRevertFailed");
                     event.setData("updateScript", script);
                     event.setDataKeyMessage("updateScript",
                        "The update script, class=" +
                        script.getClass().getName());
                     fireUpdateScriptRevertFailedEvent(event);
                  }
                  
                  // set whether or not this AutoUpdater requires a reload
                  setRequiresReload(script.autoUpdaterRequiresReload());
               }
            }
            
            // no longer processing an update
            setProcessingUpdate(false);
            
            // fire event indicating an update script was processed
            event = new EventObject("updateScriptProcessed");
            event.setData("updateScript", script);
            event.setDataKeyMessage("updateScript",
               "The update script, class=" + script.getClass().getName());
            fireUpdateScriptProcessedEvent(event);
         }
      }
   }
   
   /**
    * Overridden to terminate the auto update checker thread. 
    */
   public void finalize()
   {
      setAutoCheckForUpdate(false);
      
      if(mAutoCheckThread != null)
      {
         // make sure to interrupt the auto check thread
         mAutoCheckThread.interrupt();
      }
   }
   
   /**
    * This method is provided for convenience. It can be overloaded to
    * pause the current thread for some period of time. Another way to
    * pause between update checks is to handle the checkForUpdateStarted
    * event by pausing.
    * 
    * Causes the update checker thread to pause for some period of time
    * before checking for an update. The default period of time is
    * 30 seconds.
    * 
    * Throws an InterruptedException if the thread is interrupted while
    * sleeping. 
    * 
    * @exception InterruptedException
    */
   public void pauseUpdateCheckerThread() throws InterruptedException
   {
      if(getAutoCheckForUpdateInterval() > 0)
      {
         Thread.sleep(getAutoCheckForUpdateInterval());
      }
   }
   
   /**
    * Continuously checks for an update for the given application.
    * 
    * @param application the application to check for updates of.
    */
   public void continuouslyCheckForUpdate(AutoUpdateable application)
   {
      try
      {
         while(shouldAutoCheckForUpdate())
         {
            // pause this thread
            pauseUpdateCheckerThread();
            
            // check for an update for the application
            if(shouldAutoCheckForUpdate() &&
               !Thread.currentThread().isInterrupted())
            {
               // check for an update if the auto check interval is positive
               if(getAutoCheckForUpdateInterval() > 0)
               {
                  checkForUpdate(application);
               }
            }
         }
      }
      catch(InterruptedException e)
      {
         // interrupt thread
         Thread.currentThread().interrupt();
      }
   }
   
   /**
    * Loads an AutoUpdateable application from the passed configuration.
    * 
    * @param config the configuration to load an AutoUpdateable application
    *               from.
    *               
    * @return the AutoUpdateable application or null if one could not be loaded.
    */
   public AutoUpdateable loadAutoUpdateable(ConfigOptions config)
   {
      AutoUpdateable rval = null;
      
      try
      {
         // get the jars necessary to load the AutoUpdateable interface
         String classPath = config.getString("autoupdateable-classpath");
         String[] split = classPath.split(",");
         
         URL[] urls = new URL[split.length];
         for(int i = 0; i < urls.length; i++)
         {
            urls[i] = new URL(split[i]);
         }

         // create a class loader for the AutoUpdateable
         ClassLoader classLoader =
            new URLClassLoader(urls, getClass().getClassLoader());
         
         // load the AutoUpdateable
         Class c = classLoader.loadClass(
            config.getString("autoupdateable-class"));
         rval = (AutoUpdateable)c.newInstance();
      }
      catch(Throwable t)
      {
         getLogger().error(getClass(), Logger.getStackTrace(t));
      }
      
      return rval;
   }
   
   /**
    * Runs an application while monitoring for updates in a background process.
    * 
    * This method returns true if the application has finished executing and
    * should be run again once updates have been installed, and false if
    * the application has finished executing and should not be run again, even
    * after updates have been installed.
    * 
    * @param application the auto-updateable application to execute.
    */
   public void run(AutoUpdateable application)
   {
      // check for an update, start the application if there isn't one
      if(!checkForUpdate(application))
      {
         // set automatic check flag to true
         setAutoCheckForUpdate(true);
         
         // start the update checker thread
         Object[] params = new Object[]{application};
         MethodInvoker updateChecker =
            new MethodInvoker(this, "continuouslyCheckForUpdate", params);
         mAutoCheckThread = updateChecker;
         updateChecker.backgroundExecute();
         
         // fire event indicating that the auto-updateable application
         // is being executed
         EventObject event = new EventObject("executeApplication");
         event.setData("cancel", false);
         event.setDataKeyMessage("cancel",
            "A boolean if set to true cancels application execution.");
         fireExecuteApplicationEvent(event);
         
         // see if application execution should be cancelled
         if(!event.getDataBooleanValue("cancel"))
         {
            // execute application
            application.execute();
         }
         
         try
         {
            // sleep while the application is running
            while(application.isRunning())
            {
               Thread.sleep(1);
            }

            // interrupt update checker thread if not processing an update
            if(!isProcessingUpdate())
            {
               updateChecker.interrupt();
            }
            
            // join the update checker thread
            updateChecker.join();
         }
         catch(InterruptedException e)
         {
            // interrupt threads
            updateChecker.interrupt();
            Thread.currentThread().interrupt();
         }
         
         // set automatic check flag to false
         setAutoCheckForUpdate(false);
      }
   }
   
   /**
    * Gets the check for update started event delegate.
    * 
    * @return the check for update started event delegate.
    */
   public EventDelegate getCheckForUpdateStartedEventDelegate()
   {
      return mCheckForUpdateStartedEventDelegate;
   }
   
   /**
    * Gets the update script found event delegate.
    * 
    * @return the update script found event delegate.
    */
   public EventDelegate getUpdateScriptFoundEventDelegate()
   {
      return mUpdateScriptFoundEventDelegate;
   }
   
   /**
    * Gets the update script not found event delegate.
    * 
    * @return the update script not found event delegate.
    */
   public EventDelegate getUpdateScriptNotFoundEventDelegate()
   {
      return mUpdateScriptNotFoundEventDelegate;
   }
   
   /**
    * Gets the update script completed event delegate.
    * 
    * @return the update script completed event delegate.
    */
   public EventDelegate getUpdateScriptCompletedEventDelegate()
   {
      return mUpdateScriptCompletedEventDelegate;
   }
   
   /**
    * Gets the update script cancelled event delegate.
    * 
    * @return the update script cancelled event delegate.
    */
   public EventDelegate getUpdateScriptCancelledEventDelegate()
   {
      return mUpdateScriptCancelledEventDelegate;
   }
   
   /**
    * Gets the update script failed event delegate.
    * 
    * @return the update script failed event delegate.
    */
   public EventDelegate getUpdateScriptFailedEventDelegate()
   {
      return mUpdateScriptFailedEventDelegate;
   }

   /**
    * Gets the update script reverted event delegate.
    * 
    * @return the update script reverted event delegate.
    */
   public EventDelegate getUpdateScriptRevertedEventDelegate()
   {
      return mUpdateScriptRevertedEventDelegate;
   }

   /**
    * Gets the update script revert failed event delegate.
    * 
    * @return the update script revert failed event delegate.
    */
   public EventDelegate getUpdateScriptRevertFailedEventDelegate()
   {
      return mUpdateScriptRevertFailedEventDelegate;
   }

   /**
    * Gets the update script processed event delegate.
    * 
    * @return the update script processed event delegate.
    */
   public EventDelegate getUpdateScriptProcessedEventDelegate()
   {
      return mUpdateScriptProcessedEventDelegate;
   }
   
   /**
    * Gets the execute application event delegate.
    * 
    * @return the execute application event delegate.
    */
   public EventDelegate getExecuteApplicationEventDelegate()
   {
      return mExecuteApplicationEventDelegate;
   }
   
   /**
    * Gets the shutdown application event delegate.
    * 
    * @return the shutdown application event delegate.
    */
   public EventDelegate getShutdownApplicationEventDelegate()
   {
      return mShutdownApplicationEventDelegate;
   }
   
   /**
    * Gets the application shutdown event delegate.
    * 
    * @return the application shutdown event delegate.
    */
   public EventDelegate getApplicationShutdownEventDelegate()
   {
      return mApplicationShutdownEventDelegate;
   }
   
   /**
    * Gets whether or not this AutoUpdater requires a reload.
    * 
    * @return true if this AutoUpdater requires a reload, false if not.
    */
   public synchronized boolean requiresReload()
   {
      return mRequiresReload;
   }
   
   /**
    * Gets whether or not this AutoUpdater is processing an update.
    * 
    * @return true if this AutoUpdater is processing an update, false if not.
    */
   public synchronized boolean isProcessingUpdate()
   {
      return mProcessingUpdate;
   }
   
   /**
    * Sets the number of milliseconds to wait in between automatic update
    * checks.
    * 
    * A non-positive return value indicates that no automatic update checks
    * will be made.
    * 
    * @param interval the number of milliseconds to wait in between automatic
    *                 update checks.
    */
   public void setAutoCheckForUpdateInterval(long interval)
   {
      mAutoCheckForUpdateInterval = interval;
   }
   
   /**
    * Sets the number of milliseconds to wait in between automatic update
    * checks.
    * 
    * A non-positive return value indicates that no automatic update checks
    * will be made.
    * 
    * @return the number of milliseconds to wait in between automatic
    *         update checks.
    */
   public long getAutoCheckForUpdateInterval()
   {
      return mAutoCheckForUpdateInterval;
   }
   
   /**
    * Gets the UpdateScriptSource for this AutoUpdater. 
    * 
    * @return the UpdateScriptSource for this AutoUpdater.
    */
   public abstract UpdateScriptSource getUpdateScriptSource();
   
   /**
    * Gets the logger for this AutoUpdater.
    * 
    * @return the logger for this AutoUpdater.
    */
   public Logger getLogger()
   {
      return LoggerManager.getLogger("dbautoupdater");
   }
}
