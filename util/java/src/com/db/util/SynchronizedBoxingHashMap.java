/*
 * Copyright (c) 2006 Digital Bazaar, Inc.  All rights reserved.
 */
package com.db.util;

/**
 * A SynchronizedBoxingHashMap is a synchronized BoxingHashMap.
 * 
 * @author Dave Longley
 */
public class SynchronizedBoxingHashMap extends BoxingHashMap
{
   /**
    * Creates a new, empty, SynchronizedBoxingHashMap.
    */
   public SynchronizedBoxingHashMap()
   {
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * @param key the key of the key-value pair.
    * @return the value associated with the key.
    */
   public synchronized Object get(byte key)
   {
      return super.get(new Byte(key));
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * @param key the key of the key-value pair.
    * @return the value associated with the key.
    */
   public synchronized Object get(short key)
   {
      return super.get(new Short(key));
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * @param key the key of the key-value pair.
    * @return the value associated with the key.
    */
   public synchronized Object get(int key)
   {
      return super.get(new Integer(key));
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * @param key the key of the key-value pair.
    * @return the value associated with the key.
    */
   public synchronized Object get(long key)
   {
      return super.get(new Long(key));
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * @param key the key of the key-value pair.
    * @return the value associated with the key.
    */
   public synchronized Object get(float key)
   {
      return super.get(new Float(key));
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * @param key the key of the key-value pair.
    * @return the value associated with the key.
    */
   public synchronized Object get(double key)
   {
      return super.get(new Double(key));
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * @param key the key of the key-value pair.
    * @return the value associated with the key.
    */
   public synchronized Object get(boolean key)
   {
      return super.get(new Boolean(key));
   }

   /**
    * Gets a value from a key in this map.
    * 
    * @param key the key of the key-value pair.
    * @return the value associated with the key.
    */
   public synchronized Object get(char key)
   {
      return super.get(new Character(key));
   }

   /**
    * Puts a key-value pair in this map. The key is a byte and will
    * be boxed as a Byte.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(byte key, Object value)
   {
      super.put(new Byte(key), value);
   }

   /**
    * Puts a key-value pair in this map. The value is a byte and will
    * be boxed as a Byte.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(Object key, byte value)
   {
      super.put(key, new Byte(value));
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a byte if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a byte, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a byte.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized byte getByte(Object key)
   throws ClassCastException, NullPointerException
   {
      Byte b = (Byte)get(key);
      return b.byteValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a byte if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a byte, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a byte.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized byte getByte(byte key)
   throws ClassCastException, NullPointerException
   {
      Byte b = (Byte)get(new Byte(key));
      return b.byteValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a byte if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a byte, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a byte.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized byte getByte(short key)
   throws ClassCastException, NullPointerException
   {
      Byte b = (Byte)get(new Short(key));
      return b.byteValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a byte if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a byte, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a byte.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized byte getByte(int key)
   throws ClassCastException, NullPointerException
   {
      Byte b = (Byte)get(new Integer(key));
      return b.byteValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a byte if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a byte, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a byte.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized byte getByte(long key)
   throws ClassCastException, NullPointerException
   {
      Byte b = (Byte)get(new Long(key));
      return b.byteValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a byte if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a byte, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a byte.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized byte getByte(float key)
   throws ClassCastException, NullPointerException
   {
      Byte b = (Byte)get(new Float(key));
      return b.byteValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a byte if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a byte, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a byte.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized byte getByte(double key)
   throws ClassCastException, NullPointerException
   {
      Byte b = (Byte)get(new Double(key));
      return b.byteValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a byte if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a byte, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a byte.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized byte getByte(boolean key)
   throws ClassCastException, NullPointerException
   {
      Byte b = (Byte)get(new Boolean(key));
      return b.byteValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a byte if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a byte, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a byte.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized byte getByte(char key)
   throws ClassCastException, NullPointerException
   {
      Byte b = (Byte)get(new Character(key));
      return b.byteValue();
   }
   
   /**
    * Puts a key-value pair in this map. The key is a short and will
    * be boxed as a Short.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(short key, Object value)
   {
      super.put(new Short(key), value);
   }
   
   /**
    * Puts a key-value pair in this map. The value is a short and will
    * be boxed as a Short.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(Object key, short value)
   {
      super.put(key, new Short(value));
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a short if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a short, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a short.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized short getShort(Object key)
   throws ClassCastException, NullPointerException
   {
      Short s = (Short)get(key);
      return s.shortValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a short if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a short, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a short.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized short getShort(byte key)
   throws ClassCastException, NullPointerException
   {
      Short s = (Short)get(new Byte(key));
      return s.shortValue();
   }

   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a short if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a short, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a short.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized short getShort(short key)
   throws ClassCastException, NullPointerException
   {
      Short s = (Short)get(new Short(key));
      return s.shortValue();
   }

   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a short if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a short, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a short.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized short getShort(int key)
   throws ClassCastException, NullPointerException
   {
      Short s = (Short)get(new Integer(key));
      return s.shortValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a short if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a short, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a short.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized short getShort(long key)
   throws ClassCastException, NullPointerException
   {
      Short s = (Short)get(new Long(key));
      return s.shortValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a short if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a short, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a short.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized short getShort(float key)
   throws ClassCastException, NullPointerException
   {
      Short s = (Short)get(new Float(key));
      return s.shortValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a short if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a short, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a short.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized short getShort(double key)
   throws ClassCastException, NullPointerException
   {
      Short s = (Short)get(new Double(key));
      return s.shortValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a short if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a short, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a short.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized short getShort(boolean key)
   throws ClassCastException, NullPointerException
   {
      Short s = (Short)get(new Boolean(key));
      return s.shortValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a short if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a short, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a short.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized short getShort(char key)
   throws ClassCastException, NullPointerException
   {
      Short s = (Short)get(new Character(key));
      return s.shortValue();
   }
   
   /**
    * Puts a key-value pair in this map. The key is an integer and will
    * be boxed as an Integer.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(int key, Object value)
   {
      super.put(new Integer(key), value);
   }
   
   /**
    * Puts a key-value pair in this map. The key is an integer and will
    * be boxed as an Integer. The value is a byte and will be
    * boxed as a Byte.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(int key, byte value)
   {
      super.put(new Integer(key), new Byte(value));
   }
   
   /**
    * Puts a key-value pair in this map. The key is an integer and will
    * be boxed as an Integer. The value is a short and will be
    * boxed as a Short.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(int key, short value)
   {
      super.put(new Integer(key), new Short(value));
   }
   
   /**
    * Puts a key-value pair in this map. The key is an integer and will
    * be boxed as an Integer. The value is an integer and will be
    * boxed as an Integer.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(int key, int value)
   {
      super.put(new Integer(key), new Integer(value));
   }
   
   /**
    * Puts a key-value pair in this map. The key is an integer and will
    * be boxed as an Integer. The value is a long and will be
    * boxed as a Long.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(int key, long value)
   {
      super.put(new Integer(key), new Long(value));
   }
   
   /**
    * Puts a key-value pair in this map. The key is an integer and will
    * be boxed as an Integer. The value is a float and will be
    * boxed as a Float.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(int key, float value)
   {
      super.put(new Integer(key), new Float(value));
   }
   
   /**
    * Puts a key-value pair in this map. The key is an integer and will
    * be boxed as an Integer. The value is a double and will be
    * boxed as a Double.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(int key, double value)
   {
      super.put(new Integer(key), new Double(value));
   }
   
   /**
    * Puts a key-value pair in this map. The key is an integer and will
    * be boxed as an Integer. The value is a boolean and will be
    * boxed as a Boolean.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(int key, boolean value)
   {
      super.put(new Integer(key), new Boolean(value));
   }
   
   /**
    * Puts a key-value pair in this map. The key is an integer and will
    * be boxed as an Integer. The value is a character and will be
    * boxed as a Character.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(int key, char value)
   {
      super.put(new Integer(key), new Character(value));
   }

   /**
    * Puts a key-value pair in this map. The value is an integer and will
    * be boxed as an Integer.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(Object key, int value)
   {
      super.put(key, new Integer(value));
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into an int if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to an int, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as an int.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized int getInt(Object key)
   throws ClassCastException, NullPointerException
   {
      Integer i = (Integer)get(key);
      return i.intValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into an int if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to an int, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as an int.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized int getInt(byte key)
   throws ClassCastException, NullPointerException
   {
      Integer i = (Integer)get(new Byte(key));
      return i.intValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into an int if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to an int, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as an int.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized int getInt(short key)
   throws ClassCastException, NullPointerException
   {
      Integer i = (Integer)get(new Short(key));
      return i.intValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into an int if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to an int, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as an int.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized int getInt(int key)
   throws ClassCastException, NullPointerException
   {
      Integer i = (Integer)get(new Integer(key));
      return i.intValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into an int if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to an int, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as an int.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized int getInt(long key)
   throws ClassCastException, NullPointerException
   {
      Integer i = (Integer)get(new Long(key));
      return i.intValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into an int if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to an int, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as an int.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized int getInt(float key)
   throws ClassCastException, NullPointerException
   {
      Integer i = (Integer)get(new Float(key));
      return i.intValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into an int if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to an int, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as an int.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized int getInt(double key)
   throws ClassCastException, NullPointerException
   {
      Integer i = (Integer)get(new Double(key));
      return i.intValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into an int if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to an int, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as an int.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized int getInt(boolean key)
   throws ClassCastException, NullPointerException
   {
      Integer i = (Integer)get(new Boolean(key));
      return i.intValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into an int if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to an int, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as an int.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized int getInt(char key)
   throws ClassCastException, NullPointerException
   {
      Integer i = (Integer)get(new Character(key));
      return i.intValue();
   }
   
   /**
    * Puts a key-value pair in this map. The key is a long and will
    * be boxed as a Long.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(long key, Object value)
   {
      super.put(new Long(key), value);
   }
   
   /**
    * Puts a key-value pair in this map. The key is a long and will
    * be boxed as a Long. The value is a byte and will be boxed
    * as a Byte.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(long key, byte value)
   {
      super.put(new Long(key), new Byte(value));
   }   
   
   /**
    * Puts a key-value pair in this map. The key is a long and will
    * be boxed as a Long. The value is a short and will be boxed
    * as a Short.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(long key, short value)
   {
      super.put(new Long(key), new Short(value));
   }   
   
   /**
    * Puts a key-value pair in this map. The key is a long and will
    * be boxed as a Long. The value is an integer and will be boxed
    * as an Integer.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(long key, int value)
   {
      super.put(new Long(key), new Integer(value));
   }   
   
   /**
    * Puts a key-value pair in this map. The key is a long and will
    * be boxed as a Long. The value is a long and will be boxed
    * as a Long.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(long key, long value)
   {
      super.put(new Long(key), new Long(value));
   }   
   
   /**
    * Puts a key-value pair in this map. The key is a long and will
    * be boxed as a Long. The value is a float and will be boxed
    * as a Float.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(long key, float value)
   {
      super.put(new Long(key), new Float(value));
   }   
   
   /**
    * Puts a key-value pair in this map. The key is a long and will
    * be boxed as a Long. The value is a double and will be boxed
    * as a Double.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(long key, double value)
   {
      super.put(new Long(key), new Double(value));
   }   
   
   /**
    * Puts a key-value pair in this map. The key is a long and will
    * be boxed as a Long. The value is a boolean and will be boxed
    * as a Boolean.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(long key, boolean value)
   {
      super.put(new Long(key), new Boolean(value));
   }   
   
   /**
    * Puts a key-value pair in this map. The key is a long and will
    * be boxed as a Long. The value is a character and will be boxed
    * as a Character.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(long key, char value)
   {
      super.put(new Long(key), new Character(value));
   }   

   /**
    * Puts a key-value pair in this map. The value is a long and will
    * be boxed as a Long.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(Object key, long value)
   {
      super.put(key, new Long(value));
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a long if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a long, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a long.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized long getLong(Object key)
   throws ClassCastException, NullPointerException
   {
      Long l = (Long)get(key);
      return l.longValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a long if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a long, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a long.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized long getLong(byte key)
   throws ClassCastException, NullPointerException
   {
      Long l = (Long)get(new Byte(key));
      return l.longValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a long if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a long, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a long.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized long getLong(short key)
   throws ClassCastException, NullPointerException
   {
      Long l = (Long)get(new Short(key));
      return l.longValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a long if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a long, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a long.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized long getLong(int key)
   throws ClassCastException, NullPointerException
   {
      Long l = (Long)get(new Integer(key));
      return l.longValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a long if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a long, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a long.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized long getLong(long key)
   throws ClassCastException, NullPointerException
   {
      Long l = (Long)get(new Long(key));
      return l.longValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a long if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a long, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a long.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized long getLong(float key)
   throws ClassCastException, NullPointerException
   {
      Long l = (Long)get(new Float(key));
      return l.longValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a long if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a long, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a long.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized long getLong(double key)
   throws ClassCastException, NullPointerException
   {
      Long l = (Long)get(new Double(key));
      return l.longValue();
   }

   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a long if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a long, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a long.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized long getLong(boolean key)
   throws ClassCastException, NullPointerException
   {
      Long l = (Long)get(new Boolean(key));
      return l.longValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a long if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a long, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a long.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized long getLong(char key)
   throws ClassCastException, NullPointerException
   {
      Long l = (Long)get(new Character(key));
      return l.longValue();
   }
   
   /**
    * Puts a key-value pair in this map. The key is a float and will
    * be boxed as a Float.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(float key, Object value)
   {
      super.put(new Float(key), value);
   }

   /**
    * Puts a key-value pair in this map. The value is a float and will
    * be boxed as a Float.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(Object key, float value)
   {
      super.put(key, new Float(value));
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a float if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a float, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a float.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized float getFloat(Object key)
   throws ClassCastException, NullPointerException
   {
      Float f = (Float)get(key);
      return f.floatValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a float if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a float, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a float.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized float getFloat(byte key)
   throws ClassCastException, NullPointerException
   {
      Float f = (Float)get(new Byte(key));
      return f.floatValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a float if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a float, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a float.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized float getFloat(short key)
   throws ClassCastException, NullPointerException
   {
      Float f = (Float)get(new Integer(key));
      return f.floatValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a float if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a float, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a float.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized float getFloat(long key)
   throws ClassCastException, NullPointerException
   {
      Float f = (Float)get(new Long(key));
      return f.floatValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a float if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a float, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a float.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized float getFloat(float key)
   throws ClassCastException, NullPointerException
   {
      Float f = (Float)get(new Float(key));
      return f.floatValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a float if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a float, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a float.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized float getFloat(double key)
   throws ClassCastException, NullPointerException
   {
      Float f = (Float)get(new Double(key));
      return f.floatValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a float if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a float, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a float.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized float getFloat(boolean key)
   throws ClassCastException, NullPointerException
   {
      Float f = (Float)get(new Boolean(key));
      return f.floatValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a float if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a float, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a float.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized float getFloat(char key)
   throws ClassCastException, NullPointerException
   {
      Float f = (Float)get(new Character(key));
      return f.floatValue();
   }
   
   /**
    * Puts a key-value pair in this map. The key is a double and will
    * be boxed as a Double.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(double key, Object value)
   {
      super.put(new Double(key), value);
   }

   /**
    * Puts a key-value pair in this map. The value is a double and will
    * be boxed as a Double.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(Object key, double value)
   {
      super.put(key, new Double(value));
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a double if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a double, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a double.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized double getDouble(Object key)
   throws ClassCastException, NullPointerException
   {
      Double d = (Double)get(key);
      return d.doubleValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a double if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a double, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a double.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized double getDouble(byte key)
   throws ClassCastException, NullPointerException
   {
      Double d = (Double)get(new Byte(key));
      return d.doubleValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a double if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a double, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a double.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized double getDouble(short key)
   throws ClassCastException, NullPointerException
   {
      Double d = (Double)get(new Short(key));
      return d.doubleValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a double if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a double, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a double.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized double getDouble(int key)
   throws ClassCastException, NullPointerException
   {
      Double d = (Double)get(new Integer(key));
      return d.doubleValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a double if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a double, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a double.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized double getDouble(long key)
   throws ClassCastException, NullPointerException
   {
      Double d = (Double)get(new Long(key));
      return d.doubleValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a double if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a double, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a double.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized double getDouble(float key)
   throws ClassCastException, NullPointerException
   {
      Double d = (Double)get(new Float(key));
      return d.doubleValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a double if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a double, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a double.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized double getDouble(double key)
   throws ClassCastException, NullPointerException
   {
      Double d = (Double)get(new Double(key));
      return d.doubleValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a double if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a double, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a double.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized double getDouble(boolean key)
   throws ClassCastException, NullPointerException
   {
      Double d = (Double)get(new Boolean(key));
      return d.doubleValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a double if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a double, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a double.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized double getDouble(char key)
   throws ClassCastException, NullPointerException
   {
      Double d = (Double)get(new Character(key));
      return d.doubleValue();
   }
   
   /**
    * Puts a key-value pair in this map. The key is a boolean and will
    * be boxed as a Boolean.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(boolean key, Object value)
   {
      super.put(new Boolean(key), value);
   }

   /**
    * Puts a key-value pair in this map. The value is a boolean and will
    * be boxed as a Boolean.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(Object key, boolean value)
   {
      super.put(key, new Boolean(value));
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a boolean if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a boolean, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a boolean.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized boolean getBoolean(Object key)
   throws ClassCastException, NullPointerException
   {
      Boolean b = (Boolean)get(key);
      return b.booleanValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a boolean if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a boolean, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a boolean.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized boolean getBoolean(byte key)
   throws ClassCastException, NullPointerException
   {
      Boolean b = (Boolean)get(new Byte(key));
      return b.booleanValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a boolean if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a boolean, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a boolean.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized boolean getBoolean(int key)
   throws ClassCastException, NullPointerException
   {
      Boolean b = (Boolean)get(new Integer(key));
      return b.booleanValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a boolean if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a boolean, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a boolean.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized boolean getBoolean(long key)
   throws ClassCastException, NullPointerException
   {
      Boolean b = (Boolean)get(new Long(key));
      return b.booleanValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a boolean if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a boolean, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a boolean.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized boolean getBoolean(float key)
   throws ClassCastException, NullPointerException
   {
      Boolean b = (Boolean)get(new Float(key));
      return b.booleanValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a boolean if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a boolean, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a boolean.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized boolean getBoolean(boolean key)
   throws ClassCastException, NullPointerException
   {
      Boolean b = (Boolean)get(new Boolean(key));
      return b.booleanValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a boolean if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a boolean, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a boolean.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized boolean getBoolean(char key)
   throws ClassCastException, NullPointerException
   {
      Boolean b = (Boolean)get(new Character(key));
      return b.booleanValue();
   }
   
   /**
    * Puts a key-value pair in this map. The key is a char and will
    * be boxed as a Character.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(char key, Object value)
   {
      super.put(new Character(key), value);
   }

   /**
    * Puts a key-value pair in this map. The value is a char and will
    * be boxed as a Character.
    * 
    * @param key the key of the key-value pair.
    * @param value the value of the key-value pair.
    */
   public synchronized void put(Object key, char value)
   {
      super.put(key, new Character(value));
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a char if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a char, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a char.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized char getChar(Object key)
   throws ClassCastException, NullPointerException
   {
      Character c = (Character)get(key);
      return c.charValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a char if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a char, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a char.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized char getChar(byte key)
   throws ClassCastException, NullPointerException
   {
      Character c = (Character)get(new Byte(key));
      return c.charValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a char if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a char, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a char.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized char getChar(short key)
   throws ClassCastException, NullPointerException
   {
      Character c = (Character)get(new Short(key));
      return c.charValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a char if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a char, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a char.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized char getChar(int key)
   throws ClassCastException, NullPointerException
   {
      Character c = (Character)get(new Integer(key));
      return c.charValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a char if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a char, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a char.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized char getChar(long key)
   throws ClassCastException, NullPointerException
   {
      Character c = (Character)get(new Long(key));
      return c.charValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a char if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a char, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a char.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized char getChar(float key)
   throws ClassCastException, NullPointerException
   {
      Character c = (Character)get(new Float(key));
      return c.charValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a char if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a char, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a char.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized char getChar(double key)
   throws ClassCastException, NullPointerException
   {
      Character c = (Character)get(new Double(key));
      return c.charValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a char if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a char, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a char.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized char getChar(boolean key)
   throws ClassCastException, NullPointerException
   {
      Character c = (Character)get(new Boolean(key));
      return c.charValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a char if possible. If the key
    * is not in the map, a NullPointerException will be thrown. If the value
    * cannot be cast to a char, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a char.
    *         
    * @exception ClassCastException
    * @exception NullPointerException 
    */
   public synchronized char getChar(char key)
   throws ClassCastException, NullPointerException
   {
      Character c = (Character)get(new Character(key));
      return c.charValue();
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a String if possible. If the value
    * cannot be cast to a String, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a String.
    *         
    * @exception ClassCastException
    */
   public synchronized String getString(Object key)
   throws ClassCastException
   {
      String s = (String)get(key);
      return s;
   }
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a String if possible. If the value
    * cannot be cast to a String, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a String.
    *         
    * @exception ClassCastException
    */
   public synchronized String getString(byte key)
   throws ClassCastException
   {
      String s = (String)get(new Byte(key));
      return s;
   }   
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a String if possible. If the value
    * cannot be cast to a String, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a String.
    *         
    * @exception ClassCastException
    */
   public synchronized String getString(short key)
   throws ClassCastException
   {
      String s = (String)get(new Short(key));
      return s;
   }   
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a String if possible. If the value
    * cannot be cast to a String, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a String.
    *         
    * @exception ClassCastException
    */
   public synchronized String getString(int key)
   throws ClassCastException
   {
      String s = (String)get(new Integer(key));
      return s;
   }   
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a String if possible. If the value
    * cannot be cast to a String, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a String.
    *         
    * @exception ClassCastException
    */
   public synchronized String getString(long key)
   throws ClassCastException
   {
      String s = (String)get(new Long(key));
      return s;
   }   

   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a String if possible. If the value
    * cannot be cast to a String, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a String.
    *         
    * @exception ClassCastException
    */
   public synchronized String getString(float key)
   throws ClassCastException
   {
      String s = (String)get(new Float(key));
      return s;
   }   
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a String if possible. If the value
    * cannot be cast to a String, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a String.
    *         
    * @exception ClassCastException
    */
   public synchronized String getString(double key)
   throws ClassCastException
   {
      String s = (String)get(new Double(key));
      return s;
   }   
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a String if possible. If the value
    * cannot be cast to a String, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a String.
    *         
    * @exception ClassCastException
    */
   public synchronized String getString(boolean key)
   throws ClassCastException
   {
      String s = (String)get(new Boolean(key));
      return s;
   }   
   
   /**
    * Gets a value from a key in this map.
    * 
    * The value will be converted into a String if possible. If the value
    * cannot be cast to a String, a ClassCastException will be thrown.
    * 
    * @param key the key of the key-value pair.
    * 
    * @return the value associated with the key as a String.
    *         
    * @exception ClassCastException
    */
   public synchronized String getString(char key)
   throws ClassCastException
   {
      String s = (String)get(new Character(key));
      return s;
   }

   /**
    * Removes the mapping for this key from this map if present.
    *
    * @param  key key whose mapping is to be removed from the map.
    * 
    * @return previous value associated with specified key, or <tt>null</tt>
    *         if there was no mapping for key.  A <tt>null</tt> return can
    *         also indicate that the map previously associated <tt>null</tt>
    *         with the specified key.
    */
   public synchronized Object remove(byte key)   
   {
      return super.remove(new Byte(key));
   }
   
   /**
    * Removes the mapping for this key from this map if present.
    *
    * @param  key key whose mapping is to be removed from the map.
    * 
    * @return previous value associated with specified key, or <tt>null</tt>
    *         if there was no mapping for key.  A <tt>null</tt> return can
    *         also indicate that the map previously associated <tt>null</tt>
    *         with the specified key.
    */
   public synchronized Object remove(short key)   
   {
      return super.remove(new Short(key));
   }
   
   /**
    * Removes the mapping for this key from this map if present.
    *
    * @param  key key whose mapping is to be removed from the map.
    * 
    * @return previous value associated with specified key, or <tt>null</tt>
    *         if there was no mapping for key.  A <tt>null</tt> return can
    *         also indicate that the map previously associated <tt>null</tt>
    *         with the specified key.
    */
   public synchronized Object remove(int key)   
   {
      return super.remove(new Integer(key));
   }
   
   /**
    * Removes the mapping for this key from this map if present.
    *
    * @param  key key whose mapping is to be removed from the map.
    * 
    * @return previous value associated with specified key, or <tt>null</tt>
    *         if there was no mapping for key.  A <tt>null</tt> return can
    *         also indicate that the map previously associated <tt>null</tt>
    *         with the specified key.
    */
   public synchronized Object remove(long key)   
   {
      return super.remove(new Long(key));
   }
   
   /**
    * Removes the mapping for this key from this map if present.
    *
    * @param  key key whose mapping is to be removed from the map.
    * 
    * @return previous value associated with specified key, or <tt>null</tt>
    *         if there was no mapping for key.  A <tt>null</tt> return can
    *         also indicate that the map previously associated <tt>null</tt>
    *         with the specified key.
    */
   public synchronized Object remove(float key)   
   {
      return super.remove(new Float(key));
   }
   
   /**
    * Removes the mapping for this key from this map if present.
    *
    * @param  key key whose mapping is to be removed from the map.
    * 
    * @return previous value associated with specified key, or <tt>null</tt>
    *         if there was no mapping for key.  A <tt>null</tt> return can
    *         also indicate that the map previously associated <tt>null</tt>
    *         with the specified key.
    */
   public synchronized Object remove(double key)   
   {
      return super.remove(new Double(key));
   }
   
   /**
    * Removes the mapping for this key from this map if present.
    *
    * @param  key key whose mapping is to be removed from the map.
    * 
    * @return previous value associated with specified key, or <tt>null</tt>
    *         if there was no mapping for key.  A <tt>null</tt> return can
    *         also indicate that the map previously associated <tt>null</tt>
    *         with the specified key.
    */
   public synchronized Object remove(boolean key)   
   {
      return super.remove(new Boolean(key));
   }
   
   /**
    * Removes the mapping for this key from this map if present.
    *
    * @param  key key whose mapping is to be removed from the map.
    * 
    * @return previous value associated with specified key, or <tt>null</tt>
    *         if there was no mapping for key.  A <tt>null</tt> return can
    *         also indicate that the map previously associated <tt>null</tt>
    *         with the specified key.
    */
   public synchronized Object remove(char key)   
   {
      return super.remove(new Character(key));
   }
   
   /**
    * Returns <tt>true</tt> if this map contains a mapping for the
    * specified key.
    *
    * @param key the key whose presence in this map is to be tested.
    * @return <tt>true</tt> if this map contains a mapping for the specified
    *         key.
    */
   public synchronized boolean containsKey(byte key)   
   {
      return super.containsKey(new Byte(key));
   }
   
   /**
    * Returns <tt>true</tt> if this map contains a mapping for the
    * specified key.
    *
    * @param key the key whose presence in this map is to be tested.
    * @return <tt>true</tt> if this map contains a mapping for the specified
    *         key.
    */
   public synchronized boolean containsKey(short key)   
   {
      return super.containsKey(new Short(key));
   }
   
   /**
    * Returns <tt>true</tt> if this map contains a mapping for the
    * specified key.
    *
    * @param key the key whose presence in this map is to be tested.
    * @return <tt>true</tt> if this map contains a mapping for the specified
    *         key.
    */
   public synchronized boolean containsKey(int key)   
   {
      return super.containsKey(new Integer(key));
   }
   
   /**
    * Returns <tt>true</tt> if this map contains a mapping for the
    * specified key.
    *
    * @param key the key whose presence in this map is to be tested.
    * @return <tt>true</tt> if this map contains a mapping for the specified
    *         key.
    */
   public synchronized boolean containsKey(long key)   
   {
      return super.containsKey(new Long(key));
   }
   
   /**
    * Returns <tt>true</tt> if this map contains a mapping for the
    * specified key.
    *
    * @param key the key whose presence in this map is to be tested.
    * @return <tt>true</tt> if this map contains a mapping for the specified
    *         key.
    */
   public synchronized boolean containsKey(float key)   
   {
      return super.containsKey(new Float(key));
   }
   
   /**
    * Returns <tt>true</tt> if this map contains a mapping for the
    * specified key.
    *
    * @param key the key whose presence in this map is to be tested.
    * @return <tt>true</tt> if this map contains a mapping for the specified
    *         key.
    */
   public synchronized boolean containsKey(double key)  
   {
      return super.containsKey(new Double(key));
   }
   
   /**
    * Returns <tt>true</tt> if this map contains a mapping for the
    * specified key.
    *
    * @param key the key whose presence in this map is to be tested.
    * @return <tt>true</tt> if this map contains a mapping for the specified
    *         key.
    */
   public synchronized boolean containsKey(boolean key)   
   {
      return super.containsKey(new Boolean(key));
   }
   
   /**
    * Returns <tt>true</tt> if this map contains a mapping for the
    * specified key.
    *
    * @param key the key whose presence in this map is to be tested.
    * @return <tt>true</tt> if this map contains a mapping for the specified
    *         key.
    */
   public synchronized boolean containsKey(char key)   
   {
      return super.containsKey(new Character(key));
   }
   
   /**
    * Copies this map to another map. This method makes deep copies of
    * boxed primitive values.
    * 
    * @return the copied map.
    */
   public synchronized BoxingHashMap copy()
   {
      SynchronizedBoxingHashMap copy = new SynchronizedBoxingHashMap();
      
      // copy values from this map
      copy.copyFrom(this);
      
      // return copy
      return copy;
   }
}
