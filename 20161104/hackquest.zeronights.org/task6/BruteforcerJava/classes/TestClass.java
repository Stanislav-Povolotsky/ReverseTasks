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
        Cipher cipher = Cipher.getInstance("AES/ECB/NoPadding");
        SimpleDateFormat simpleDateFormat = new SimpleDateFormat("MM/dd/yyyy HH:mm");
        Date date = simpleDateFormat.parse("10/28/2016 00:00:00");
        long t = date.getTime();
        Date dateEnd = simpleDateFormat.parse("10/27/2016 00:00:00");
        long tEnd = dateEnd.getTime();
        // Первые 16 байт зашифрованного файла
        byte[] src_data = {
            (byte)0xDE,(byte)0x9E,(byte)0x2A,(byte)0x63,(byte)0xF9,(byte)0x8D,(byte)0xFE,(byte)0x0B,
            (byte)0x7F,(byte)0x7E,(byte)0x4B,(byte)0x13,(byte)0x19,(byte)0xD7,(byte)0xF5,(byte)0x4E};
        while(t >= tEnd)
        {
            t = t - 1;
            if((t % 100000) == 0) {
              System.out.format("Cur key %d\n", t);
            }
            byte[] key = generateKey(t);
            SecretKeySpec keyWrap = new SecretKeySpec(key, 0, 16, "AES");
            cipher.init(Cipher.DECRYPT_MODE, keyWrap);
            byte[] decryptedBytes = cipher.doFinal(src_data);
            // Проверка первых трёх байт расшифрованного файла
            if((decryptedBytes[0] & 0xFF) == 0xFF && 
               (decryptedBytes[1] & 0xFF) == 0xD8 && 
               (decryptedBytes[2] & 0xFF) == 0xFF)
            {
              System.out.format("Done %d\n", t);
              writeFile(decryptedBytes, String.format("data\\decrypted_%d", t));
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

