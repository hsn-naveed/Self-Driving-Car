package undergradsplusplus.ug;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.util.Log;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Set;
import java.util.UUID;

/**
 * Created by ming on 11/10/15.
 */
public class BluetoothConnect extends Thread {

    BluetoothAdapter mBluetoothAdapter;
    static BluetoothSocket mBluetoothSocket;
    public static final String MAINACTIVITY_TAG = "Bluetooth connect";

    public static InputStream mmInputStream;
    public static OutputStream mmOutputStream;
    public final static int REQ_BLUETOOTH_EN = 1;

    static boolean connectionEstablished;

    Activity activity;
    Context context;

    BluetoothConnect(BluetoothSocket socket, Activity activity, Context context){
        this.activity = activity;
        this.context = context;
        mBluetoothSocket = socket;
    }

    boolean getBluetoothConnection(){

        ArrayList<BluetoothDevice> deviceArrayList = new ArrayList<BluetoothDevice>();
        connectionEstablished = false;

        final UUID WELL_KNOW_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        // To check if Bluetooth is enabled
        if(!(mBluetoothAdapter.isEnabled())){
            Intent turnOn = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            activity.startActivityForResult(turnOn, REQ_BLUETOOTH_EN);
        } else {

            Toast.makeText(context, "Bluetooth Already ON", Toast.LENGTH_LONG).show();

        }

        Intent discoverable = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);

        Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();
        if(pairedDevices.size() > 0){
            for(BluetoothDevice device : pairedDevices){
                Log.d(MAINACTIVITY_TAG + "Device", device.getUuids().toString() + "\n" + device.getAddress());
                deviceArrayList.add(device);
            }
        }
        BluetoothDevice ourDevice = deviceArrayList.get(0);
        Log.e(MAINACTIVITY_TAG, ourDevice.getAddress().toString());
        try {
            mBluetoothSocket = ourDevice.createRfcommSocketToServiceRecord(WELL_KNOW_UUID);
            Log.e(MAINACTIVITY_TAG + "", WELL_KNOW_UUID.toString());
            Method method = ourDevice.getClass().getMethod("createRfcommSocket", new Class[]{int.class});

        }catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        Log.i(MAINACTIVITY_TAG, "Trying to connect.......");
        mBluetoothAdapter.cancelDiscovery();

        try {
            mBluetoothSocket.connect();
            Log.i(MAINACTIVITY_TAG, "Connection established");
            connectionEstablished = true;
        } catch (IOException e) {
            Log.e(MAINACTIVITY_TAG, "Could not connect!!!!!");
            e.printStackTrace();
            connectionEstablished = false;
        }
        Boolean isConnected = mBluetoothSocket.isConnected();
        if(isConnected){
            Log.e(MAINACTIVITY_TAG, "BLUETOOTH CONNECTED");
            connectionEstablished = true;
        }

        InputStream tmpIn = null;
        OutputStream tmpOut = null;

        try{
            tmpIn = mBluetoothSocket.getInputStream();
            tmpOut = mBluetoothSocket.getOutputStream();
        } catch (IOException e){
            e.printStackTrace();
        }

        BluetoothConnect.mmOutputStream = tmpOut;
        BluetoothConnect.mmInputStream = tmpIn;

        return connectionEstablished;

    }
}
