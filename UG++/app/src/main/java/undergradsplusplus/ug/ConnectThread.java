package undergradsplusplus.ug;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.util.Log;

import java.io.IOException;
import java.util.UUID;

/**
 * Created by PhiTran on 11/3/15.
 * Creating
 */
public class ConnectThread extends Thread{
    private final BluetoothSocket mmSocket;
    private final BluetoothDevice mmDevice;

    public static BluetoothAdapter mBluetoothAdapter;

    private final static String APP_ID = "Connect Thread";
    private final static UUID UG_UUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");    // Standard Bluetooth UUID, RFCOMM

//    BTSocket btSocket = new BTSocket() {
//        @Override
//        public void getSocket(BluetoothSocket socket) {
//            Log.d(APP_ID, "in getSocket interface");
//        }
//    };

    BTSocket btSocket;

    public interface BTSocket {
        void getSocket(BluetoothSocket socket);
    }

    public ConnectThread (BluetoothDevice device, BluetoothAdapter bluetoothAdapter, Activity activity)
    {
        BluetoothSocket tmp = null;
        mmDevice = device;
        mBluetoothAdapter = bluetoothAdapter;
        try
        {
            btSocket = (BTSocket) activity; // allows the BTSocket implementation to go to Activity
            tmp = device.createInsecureRfcommSocketToServiceRecord(UG_UUID);
        } catch (IOException e) {
            e.printStackTrace();
        }
        mmSocket = tmp;

    }

    public void run ()
    {
        // Cancel discovery because it will slow down the connection
        mBluetoothAdapter.cancelDiscovery();

        try {
            // Connect the device through the socket. This will block
            // until it succeeds or throws an exception
            Log.d("CONNECT", "OPEN CONNECTED");
            btSocket.getSocket(mmSocket);
            mmSocket.connect();
        } catch (IOException connectException) {
            // Unable to connect; close the socket and get out
            try {
                Log.d("CONNECT", "CLOSE CONNECTED");
                mmSocket.close();
            } catch (IOException closeException) {
                Log.d("CONNECT", "IS NOT CONNECTED");
                Log.e(APP_ID + " A", Log.getStackTraceString(closeException));
            }

            Log.e(APP_ID + " B", Log.getStackTraceString(connectException));
        }

        ConnectedThread mConnectedThread = new ConnectedThread(mmSocket);
        mConnectedThread.start();

    }

    /** Will cancel an in-progress connection, and close the socket */
    public void cancel() {
        try {
            mmSocket.close();
        } catch (IOException e) { }
    }


}