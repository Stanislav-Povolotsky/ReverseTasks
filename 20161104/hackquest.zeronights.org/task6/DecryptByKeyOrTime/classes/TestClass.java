package classes;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;


import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import static java.nio.file.Files.*;
import java.nio.file.Path;
import static java.nio.file.Paths.get;
import java.nio.file.StandardOpenOption;
import java.security.MessageDigest;
import java.security.Provider;
import java.security.SecureRandom;
import java.security.Security;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import java.util.stream.LongStream;
import javax.crypto.Cipher;
import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;
import javax.crypto.spec.SecretKeySpec;

//import android.content.ContentValues;
//import android.database.Cursor;
//import android.database.sqlite.SQLiteDatabase;
//import android.database.sqlite.SQLiteOpenHelper;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

public class TestClass {
    public static byte[] readFile(String file) {
        try (FileInputStream in = new FileInputStream(file)) {
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            transfer(in, out);
            return out.toByteArray();
        } catch (IOException io) {
            throw new RuntimeException(io);
        }
    }
    public static void transfer(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[4096];
        int status;
        while ((status = in.read(buffer)) >= 0) {
            out.write(buffer, 0, status);
        }
    }

    public static void writeFile(byte[] data, String file) {
        try (FileOutputStream out = new FileOutputStream(file)) {
            ByteArrayInputStream in = new ByteArrayInputStream(data);
            transfer(in, out);
        } catch (IOException io) {
            throw new RuntimeException(io);
        }
    }

    public static void main(String[] args) throws Exception {
        /*if (args.length < 2) {
            System.out.println("Usage: java -cp <jar> Keygen <input long number> <output key file>");
            System.exit(0);
        }*/
        Date date = new Date();
        long t = 1478187467769L; //date.getTime();

        // Long.parseLong(args[0]))
        // write(get(args[1]), generateKey(t));
        byte[] src_data = readFile("data\\pack_value.4.bin");
        //byte[] src_data = readFile("data\\pack_value.4.small.bin");
        //byte[] src_data = {(byte)0xDE,(byte)0x9E,(byte)0x2A,(byte)0x63,(byte)0xF9,(byte)0x8D,(byte)0xFE,(byte)0x0B,(byte)0x7F,(byte)0x7E,(byte)0x4B,(byte)0x13,(byte)0x19,(byte)0xD7,(byte)0xF5,(byte)0x4E};

        byte[] iv = new byte[16];
        //SecureRandom random = new SecureRandom();
        //random.nextBytes(iv);
        IvParameterSpec ivParameterSpec = new IvParameterSpec(iv);

        //System.out.format("Key %d\n", t);
        //byte[] key = generateKey(t);
        byte[] key = readFile("data\\key");

        //SecretKeySpec keyWrap = new SecretKeySpec(key, "AES");
        SecretKeySpec keyWrap = new SecretKeySpec(key, 0, 16, "AES");
        //Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
        //cipher.init(Cipher.DECRYPT_MODE, keyWrap, ivParameterSpec);
        Cipher cipher = Cipher.getInstance("AES/ECB/NoPadding");
        cipher.init(Cipher.DECRYPT_MODE, keyWrap);
        byte[] decryptedBytes = cipher.doFinal(src_data);
        writeFile(decryptedBytes, String.format("data\\test_decrypted", t));
        writeFile(key, String.format("data\\key"));
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

