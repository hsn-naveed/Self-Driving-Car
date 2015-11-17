package undergradsplusplus.ug;

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
    public BluetoothAdapter mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    ArrayAdapter<String> mArrayAdapter;
    public static ListView newDevList;
    public final String MAC = "34:36:3B:CD:11:D5";

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.setup_layout, container, false);

        Button ENABLE = (Button) v.findViewById(R.id.enable_button);
        ENABLE.setOnClickListener(this);
        Button DISCOVER = (Button) v.findViewById(R.id.discover_button);
        DISCOVER.setOnClickListener(this);

        return v;
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

        text = (TextView) getActivity().findViewById(R.id.text_setup);
        newDevList = (ListView) getActivity().findViewById(android.R.id.list);
        Log.d("phil", "in setup");
    }



    @Override
    public void onClick(View v) {

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

            case R.id.discover_button:
                IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
                getActivity().registerReceiver(mReceiver, filter); // Don't forget to unregister during onDestroy
                break;

            case R.id.connect_button:
                if (mBluetoothAdapter.checkBluetoothAddress(MAC)) {
                    BluetoothDevice mDevice = mBluetoothAdapter.getRemoteDevice(MAC);
                    ConnectThread connectThread = new ConnectThread(mDevice, mBluetoothAdapter);
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




}
