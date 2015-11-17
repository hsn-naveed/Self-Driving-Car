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
    public static Activity activity;

    private final static UUID UG_UUID = UUID.fromString("71e24236-1cb1-4c53-9217-93933abe4680");

    public ConnectThread (BluetoothDevice device, BluetoothAdapter bluetoothAdapter)
    {
        BluetoothSocket tmp = null;
        mmDevice = device;
        mBluetoothAdapter = bluetoothAdapter;
        try
        {
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
            mmSocket.connect();
        } catch (IOException connectException) {
            // Unable to connect; close the socket and get out
            try {
                Log.d("CONNECT", "CLOSE CONNECTED");
                mmSocket.close();
            } catch (IOException closeException) {
                Log.d("CONNECT", "IS NOT CONNECTED");
            }
        }

    }

    /** Will cancel an in-progress connection, and close the socket */
    public void cancel() {
        try {
            mmSocket.close();
        } catch (IOException e) { }
    }

}
