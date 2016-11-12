package com.example.test1;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;


import java.io.IOException;
import java.nio.ByteBuffer;
import java.security.MessageDigest;
import java.security.Provider;
import java.security.SecureRandom;
import java.security.Security;
import java.text.Format;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import javax.crypto.Cipher;
import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;
import javax.crypto.spec.SecretKeySpec;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;

public class MainActivity extends Activity {
	static TextView s_textView;
	static long s_cur_t = 0;
	static long s_cur_found = 0;
	static String s_storageBase;
    static Handler s_handler;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		s_storageBase = "/mnt/sdcard";
		String primary_sd = System.getenv("EXTERNAL_STORAGE");
		if(primary_sd != null)
			s_storageBase = primary_sd;
		String secondary_sd = System.getenv("SECONDARY_STORAGE");
		if(secondary_sd != null)
			s_storageBase = secondary_sd;
		    
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		s_textView = (TextView) findViewById(R.id.textView03);
		s_textView.setText(s_storageBase);
		s_handler = new Handler();
		
		Thread thread = new Thread() {
		    @Override
		    public void run() {
				try {
					mainBrute();
					//mainTest1();
				} catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
		    }
		};

		thread.start();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}
	
	public static String getStringFromFile (String filePath) throws Exception {
	    File fl = new File(filePath);
	    FileInputStream fin = new FileInputStream(fl);
	    String ret = convertStreamToString(fin);
	    //Make sure you close all streams.
	    fin.close();        
	    return ret;
	}

	public static String convertStreamToString(InputStream is) throws Exception { 
	    BufferedReader reader = new BufferedReader(new InputStreamReader(is));
	    StringBuilder sb = new StringBuilder();
	    String line = null;
	    boolean bFirst = true;
	    while ((line = reader.readLine()) != null) {
	      if(!bFirst) sb.append("\n");
	      bFirst = false;
	      sb.append(line);
	    }
	    reader.close();
	    return sb.toString();
	}

    public static void writeFile(byte[] data, String file) throws IOException 
    {
    	BufferedOutputStream bos = new BufferedOutputStream(new FileOutputStream(file));
    	bos.write(data);
    	bos.flush();
    	bos.close();
    }

    public static void mainTest1() throws Exception 
    {
        /*if (args.length < 2) {
            System.out.println("Usage: java -cp <jar> Keygen <input long number> <output key file>");
            System.exit(0);
        }*/
        Date date = new Date();
        long t = 1478187467769L; //date.getTime();

        // Long.parseLong(args[0]))
        // write(get(args[1]), generateKey(t));
        //byte[] src_data = readFile("data\\pack_value.4.bin");
        //byte[] src_data = readFile("data\\pack_value.4.small.bin");
        byte[] src_data = {(byte)0xDE,(byte)0x9E,(byte)0x2A,(byte)0x63,(byte)0xF9,(byte)0x8D,(byte)0xFE,(byte)0x0B,(byte)0x7F,(byte)0x7E,(byte)0x4B,(byte)0x13,(byte)0x19,(byte)0xD7,(byte)0xF5,(byte)0x4E};

        byte[] iv = new byte[16];
        //SecureRandom random = new SecureRandom();
        //random.nextBytes(iv);
        IvParameterSpec ivParameterSpec = new IvParameterSpec(iv);

        //System.out.format("Key %d\n", t);
        byte[] key = generateKey(t);
        //SecretKeySpec keyWrap = new SecretKeySpec(key, "AES");
        SecretKeySpec keyWrap = new SecretKeySpec(key, 0, 16, "AES");
        //Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
        //cipher.init(Cipher.DECRYPT_MODE, keyWrap, ivParameterSpec);
        Cipher cipher = Cipher.getInstance("AES/ECB/NoPadding");
        cipher.init(Cipher.DECRYPT_MODE, keyWrap);
        byte[] decryptedBytes = cipher.doFinal(src_data);
        writeFile(decryptedBytes, String.format(s_storageBase + "/temp/test_decrypted", t));
        writeFile(key, String.format(s_storageBase + "/temp/key"));
    }
    
    public static void mainBrute() throws Exception {
        /*if (args.length < 2) {
            System.out.println("Usage: java -cp <jar> Keygen <input long number> <output key file>");
            System.exit(0);
        }*/
        //Date date = new Date();
        SimpleDateFormat simpleDateFormat = new SimpleDateFormat("MM/dd/yyyy HH:mm");
        Date date = simpleDateFormat.parse("11/06/2016 00:00:00");
        long t = date.getTime();
        
        try
        {
        	String sLast = getStringFromFile(String.format(s_storageBase + "/temp/state.txt"));
        	long last_t = Long.parseLong(sLast, 10);
        	t = last_t;
        }
        catch(Exception e)
        {
            writeFile(e.getMessage().getBytes(), String.format(s_storageBase + "/temp/last_exception.txt"));
        }

        // Long.parseLong(args[0]))
        // write(get(args[1]), generateKey(t));
        //byte[] src_data = readFile("data\\pack_value.4.bin");
        //byte[] src_data = readFile("data\\pack_value.4.small.bin");
        byte[] src_data = {(byte)0xDE,(byte)0x9E,(byte)0x2A,(byte)0x63,(byte)0xF9,(byte)0x8D,(byte)0xFE,(byte)0x0B,(byte)0x7F,(byte)0x7E,(byte)0x4B,(byte)0x13,(byte)0x19,(byte)0xD7,(byte)0xF5,(byte)0x4E};

        byte[] iv = new byte[16];
        //SecureRandom random = new SecureRandom();
        //random.nextBytes(iv);
        IvParameterSpec ivParameterSpec = new IvParameterSpec(iv);

            Cipher cipher = 
		//Cipher.getInstance("AES/CBC/PKCS5Padding");
		Cipher.getInstance("AES/ECB/NoPadding");
        t = t - 1;
        boolean bFirst = true;

        while(t > 0)
        {
        try {
            t = t + 1;
            if(((t % 100000) == 0) || bFirst) {
            	s_cur_t = t;
            	bFirst = false;
            	s_handler.post(new Runnable(){
                    public void run() {
                    	try
                    	{
		                     Date date = new Date(s_cur_t);
		                     Format format = new SimpleDateFormat("yyyy MM dd HH:mm:ss");
		                  	 s_textView.setText(String.format("Cur key %d (%s)\nFound: %d", s_cur_t, format.format(date), s_cur_found));
                    	}
                    	catch(Exception e){}
                   }
                    	
              });

              String state = String.format("%d", t);
              writeFile(state.getBytes(), String.format(s_storageBase + "/temp/state.txt"));
            }
            byte[] key = generateKey(t);
            SecretKeySpec keyWrap = new SecretKeySpec(key, 0, 16, "AES");
            cipher.init(Cipher.DECRYPT_MODE, keyWrap /*, ivParameterSpec*/);
            byte[] decryptedBytes = cipher.doFinal(src_data);
            if((decryptedBytes[0] & 0xFF) == 0xFF && (decryptedBytes[1] & 0xFF) == 0xD8 && (decryptedBytes[2] & 0xFF) == 0xFF)
            {
             System.out.format("Done %d\n", t);
             s_cur_found += 1;
             writeFile(decryptedBytes, String.format(s_storageBase + "/temp/decrypted_%d", t));
             writeFile(key, String.format(s_storageBase + "/temp/key_%d", t));
            }
            //break;
            
        } catch (Exception io) {
            continue;
        }
        }
    }

    public static byte[] generateKey(long input) throws Exception {
        MessageDigest digest = MessageDigest.getInstance("SHA-256");
        digest.update(ByteBuffer.allocate(8).putLong(input).array());
        Provider provider = Security.getProviders("SecureRandom.SHA1PRNG")[0];
        SecureRandom secureRandom = SecureRandom.getInstance("SHA1PRNG", provider);
        secureRandom.setSeed(digest.digest());
        KeyGenerator keyGen = KeyGenerator.getInstance("AES");
        keyGen.init(128, secureRandom);
        SecretKey secretKey = keyGen.generateKey();
        return secretKey.getEncoded();
    }
}

