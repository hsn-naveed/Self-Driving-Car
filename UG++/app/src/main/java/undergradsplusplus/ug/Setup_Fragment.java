package undergradsplusplus.ug;

import android.app.Activity;
import android.app.Fragment;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;


/**
 * Created by PhiTran on 10/24/15.
 */
public class Setup_Fragment extends Fragment implements View.OnClickListener{

    TextView text;
    BluetoothAdapter mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    ArrayAdapter<String> mArrayAdapter;
    public static ListView newDevList;
//    public final String MAC = "00:6A:8E:16:C3:00";    //UG++
//    public final String MAC = "34:36:3B:CD:11:D5";
    public final String MAC = "20:14:03:28:28:60";
    public Activity activity;



    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.setup_layout, container, false);

        Button ENABLE = (Button) v.findViewById(R.id.enable_button);
        ENABLE.setOnClickListener(this);
        Button DISCOVER = (Button) v.findViewById(R.id.discover_button);
        DISCOVER.setOnClickListener(this);
        Button CONNECT = (Button) v.findViewById(R.id.connect_button);
        CONNECT.setOnClickListener(this);

        return v;
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

        text = (TextView) getActivity().findViewById(R.id.text_setup);  // TITLE
        newDevList = (ListView) getActivity().findViewById(android.R.id.list);  // ListView of available devices
        Log.d("phil", "in setup");
    }



    @Override
    public void onClick(View v) {

        // Enable bluetooth and/or see available devices
        switch(v.getId())
        {
            case R.id.enable_button:
                if (mBluetoothAdapter == null) {
                    Log.i("BT", "No Adapter");
                }

                if (!mBluetoothAdapter.isEnabled()) {
                    Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    startActivityForResult(enableBtIntent, 1);
                }

                getPairedDevices();

                break;

            // Discover other devices. This button probably doesn't work....
            case R.id.discover_button:
                IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
                getActivity().registerReceiver(mReceiver, filter); // Don't forget to unregister during onDestroy
                break;

            // Connect to device via MAC address. MAC address hardcoded for now. Connect from list if time permits.
            case R.id.connect_button:
                BluetoothDevice mDevice = mBluetoothAdapter.getRemoteDevice(MAC);
                Log.d("BEFORE CHECK", MAC);
                if (mBluetoothAdapter.checkBluetoothAddress(MAC)) {
                    activity = getActivity();

                    //Must pass activity so that ConnectThread can
                    ConnectThread connectThread = new ConnectThread(mDevice, mBluetoothAdapter, activity);
                    connectThread.start();
                    Log.d("MAC ADDRESS", MAC);
                }
                break;
        }


    }

    public void getPairedDevices()
    {
        List <String> s = new ArrayList<String>();
        mArrayAdapter = new ArrayAdapter<String>(getActivity(), android.R.layout.simple_list_item_1, s);
        Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();
        // If there are paired devices
        if (pairedDevices.size() > 0) {
            // Loop through paired devices
            for (BluetoothDevice device : pairedDevices) {
                // Add the name and address to an array adapter to show in a ListView
                s.add(device.getName() + "\n" + device.getAddress());
            }
            newDevList.setAdapter(mArrayAdapter);
        }
    }

    // Create a BroadcastReceiver for ACTION_FOUND
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            // When discovery finds a device
            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                // Get the BluetoothDevice object from the Intent
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                // Add the name and address to an array adapter to show in a ListView
                mArrayAdapter.add(device.getName() + "\n" + device.getAddress());
            }
        }
    };

    @Override
    public void onDestroy() {
        super.onDestroy();

        getActivity().unregisterReceiver(mReceiver);
    }
}
