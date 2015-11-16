package undergradsplusplus.ug;

import android.bluetooth.BluetoothSocket;
import android.util.Log;

import java.io.IOException;

/**
 * Created by ming on 11/15/15.
 */
public class BluetoothRecieve {
    public static final String MAINACTIVITY_TAG = "Bluetooth Receive";
    int readBufferedPosition = 0;
    byte [] readBuffer;
    volatile boolean stopWorker;
    Thread workerThread;
    BluetoothSocket socket = BluetoothConnect.mBluetoothSocket;


    synchronized void listenForData(){

        stopWorker = false;
        readBufferedPosition = 0;
        readBuffer = new byte[50];
        final byte delimiter = (byte)'$';


        workerThread = new Thread(new Runnable() {
            @Override
            public void run() {
                while(!Thread.currentThread().isInterrupted() && !stopWorker){
                    //Log.d(MAINACTIVITY_TAG, "Inside while");
                    try{
                        int bytesAvailable = BluetoothConnect.mmInputStream.available();
                        //Log.i(MAINACTIVITY_TAG, "Bytes Available : " + bytesAvailable);
                        if(bytesAvailable > 0){
                            byte [] packetBytes = new byte[bytesAvailable];
                            int available = BluetoothConnect.mmInputStream.read(packetBytes);
                            Log.e(MAINACTIVITY_TAG, "Received: " + available);
//                            final String data = new String(packetBytes, "US-ASCII");
//                            Log.e(MAINACTIVITY_TAG, "Data: " + data);
                            for(int i=0; i<bytesAvailable; i++){
                                byte b = packetBytes[i];
                                if(b==delimiter){
                                    byte [] encodedBytes = new byte[readBufferedPosition];
                                    System.arraycopy(readBuffer, 0, encodedBytes, 0, encodedBytes.length);
                                    String data = new String(encodedBytes, "UTF-8");

                              //      Log.e(MAINACTIVITY_TAG + "Received:", data + '\n');
                                    data = null;
                                    readBufferedPosition = 0;

                                }
                                else{
                                    readBuffer[readBufferedPosition++] = b;
                                }
                            }
                        }
                    } catch (IOException e){
                        Log.e(MAINACTIVITY_TAG, "Cannot read bytes");
                        stopWorker = true;
                    }
                }
            }
        });
        workerThread.start();
    }
}
