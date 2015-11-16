package undergradsplusplus.ug;

import android.bluetooth.BluetoothSocket;
import android.os.AsyncTask;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Created by ming on 11/10/15.
 */
public class BluetoothSend extends Thread {

    public static final String MAINACTIVITY_TAG = "Bluetooth Send";
    String sendBuffer;

    BluetoothSend(String buffer)
    {
        sendBuffer = buffer;
    }

    public void run(){

        byte [] buffer = sendBuffer.getBytes();
        try{

            BluetoothConnect.mmOutputStream.write(buffer);
            BluetoothConnect.mmOutputStream.flush();
            Log.e(MAINACTIVITY_TAG, "Data send successful");
        } catch (IOException e){
            Log.e(MAINACTIVITY_TAG, "Error data send");
        }
    }

}
