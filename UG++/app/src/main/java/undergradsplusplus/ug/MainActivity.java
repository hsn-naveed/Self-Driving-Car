package undergradsplusplus.ug;


import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.bluetooth.BluetoothSocket;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.View;

import com.google.android.gms.maps.model.LatLng;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

/*
*      TODO 1:       Implement Setup, Map, and then Status Fragments
*      TODO 1.2:     Establish Bluetooth communication
*      TODO 2:       Implement cross-fragment interaction
*      TODO 2.2:     Google Maps capabilities
*      TODO 3:       Establish CAN communication
*      TODO 4:       Parse CAN ID's
*/

public class MainActivity extends FragmentActivity implements Map_Fragment.sendPointsToActivity, ConnectThread.BTSocket {

    private BluetoothSocket mmSocket;
    private final String APP_ID = "MainActivity";

    FragmentManager manager;
    FragmentTransaction transaction;

    Setup_Fragment setupFrag = new Setup_Fragment();
    Map_Fragment mapFrag = new Map_Fragment();
    Status_Fragment statusFrag = new Status_Fragment();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        /*
        *   This section of onCreate will initialize all the fragments.
        *   After adding, it will hide the fragments. The user will put
        *   the fragments to the foreground by pressing the buttons on the bottom.
         */
        manager = getFragmentManager();

        transaction = manager.beginTransaction();
        transaction.add(R.id.fragment_layout, setupFrag, "SETUP");
        transaction.add(R.id.fragment_layout, mapFrag, "MAP");
        transaction.add(R.id.fragment_layout, statusFrag, "STATUS");
        transaction.hide(setupFrag);
        transaction.hide(mapFrag);
        transaction.hide(statusFrag);
        transaction.commit();



        Log.d("onMAIN", "IN MAIN NOW");
    }


    /*
    *   Setup Button. Created in XML.
     */
    public void setupButton(View v)
    {
        if(setupFrag.isHidden())
        {
            manager.beginTransaction().show(setupFrag).commit();

            if(mapFrag.isVisible())
                manager.beginTransaction().hide(mapFrag).commit();
            else if(statusFrag.isVisible())
                manager.beginTransaction().hide(statusFrag).commit();

        }
        else
            manager.beginTransaction().hide(setupFrag).commit();

    }

    /*
    *   Map Button. Created in XML.
     */
    public void mapButton(View v)
    {
        if(mapFrag.isHidden())
        {
            manager.beginTransaction().show(mapFrag).commit();

            if(setupFrag.isVisible())
                manager.beginTransaction().hide(setupFrag).commit();
            else if(statusFrag.isVisible())
                manager.beginTransaction().hide(statusFrag).commit();

        }
        else
            manager.beginTransaction().hide(mapFrag).commit();
    }

    /*
    *   Status Button. Created in XML.
     */
    public void statusButton(View v)
    {
        if(statusFrag.isHidden())
        {
            manager.beginTransaction().show(statusFrag).commit();

            if(setupFrag.isVisible())
                manager.beginTransaction().hide(setupFrag).commit();
            else if(mapFrag.isVisible())
                manager.beginTransaction().hide(mapFrag).commit();
        }
        else
            manager.beginTransaction().hide(statusFrag).commit();
    }

    @Override
    public void transmitPoints(List<LatLng> dirPoints) {
        List<LatLng> newDir = new ArrayList<LatLng>(dirPoints);
        byte[] Lat;
        byte[] Long;
        ConnectedThread connectedThread = new ConnectedThread(mmSocket);

        Log.d("ACTIVITY DIR SIZE", "" + newDir.size());
        for (int i = 0; i < newDir.size(); i++)
        {
            // Put latitude and longitude into their own separate byte arrays.
            Lat = ByteBuffer.allocate(4).putFloat((float) newDir.get(i).latitude).array();
            Long = ByteBuffer.allocate(4).putFloat((float) newDir.get(i).longitude).array();

            // used to test the Endianness and order of the bytes
//            float f = ByteBuffer.wrap(Lat).order(ByteOrder.BIG_ENDIAN).getFloat();
//            float g = ByteBuffer.wrap(Long).order(ByteOrder.BIG_ENDIAN).getFloat();
//
//            Log.d(APP_ID + " LAT", "" + f);
//            Log.d(APP_ID + " LONG", "" + g);

            synchronized (this) {
                // Sends to thread to write. catArray(Lat, Long) concatenates Lat/Lng arrays to a single byte array.
                connectedThread.write(catArray(Lat, Long));
            }
        }

    }

    @Override
    public void goSignal(int i) {
        byte[] sendGo = "GO".getBytes();
//        byte[] sendGo = ByteBuffer.allocate(1).putInt(i).array();   // 1 byte, i = 1 for Go.
        for (int j = 0; j < sendGo.length; j++)
        {
            Log.d(APP_ID + " GO", "" + sendGo[j]);
        }
        ConnectedThread mConnectedThread = new ConnectedThread(mmSocket);
        mConnectedThread.write(sendGo);
    }

    @Override
    public void stopSignal(int i) {
        byte [] sendStop = "STOP".getBytes();
//        byte [] sendStop = ByteBuffer.allocate(1).putInt(i).array();    // 1 byte, i = 0 for Stop.
        for (int j = 0; j < sendStop.length; j++)
        {
            Log.d(APP_ID + " STOP", "" + sendStop[j]);
        }
        ConnectedThread mConnectedThread = new ConnectedThread(mmSocket);
        mConnectedThread.write(sendStop);
    }

    @Override
    public void getSocket(BluetoothSocket socket) {
        Log.d("In getSocket", "in here");

        if (socket != null)
            mmSocket = socket;
        Log.d("NULL socket", "NULL");
    }

    public byte[] catArray(byte[] Lat, byte[] Long)
    {
        byte[] cat = new byte[8];

        System.arraycopy(Lat, 0, cat, 0, 4);
        System.arraycopy(Long, 0, cat, 4, 4);
        return cat;
    }

//    public void byteToFloatTest(byte[] Lat, byte[] Long)
//    {
//        byte[] bytes = catArray(Lat, Long);
//        int asInt = ((bytes[0] & 0xFF) << 24)
//                | ((bytes[1] & 0xFF) << 16)
//                | ((bytes[2] & 0xFF) << 8)
//                | ((bytes[3] & 0xFF));
//        int asInt2 = ((bytes[4] & 0xFF) << 24)
//                | ((bytes[5] & 0xFF) << 16)
//                | ((bytes[6] & 0xFF) << 8)
//                | ((bytes[7] & 0xFF));
//
//        float gg = Float.intBitsToFloat(asInt);
//        float hh = Float.intBitsToFloat(asInt2);
//
//
//        Log.d(APP_ID + " bytes", "" + gg);
//        Log.d(APP_ID + " bytes", "" + hh);
//
//    }

}
