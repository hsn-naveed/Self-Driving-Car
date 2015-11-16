package undergradsplusplus.ug;


import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.View;

import com.google.android.gms.maps.model.LatLng;

import java.text.DecimalFormat;
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

public class MainActivity extends FragmentActivity implements Map_Fragment.sendPointsToActivity {

    private static List<LatLng> newDir;
    private static List<LatLng> formattedDirections;

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
        newDir = new ArrayList<LatLng>(dirPoints);
        DecimalFormat df = new DecimalFormat("#.000000");

        Log.d("IN transmitPoints", "Transmit Points");

        Log.d("ACTIVITY DIR SIZE", "" + newDir.size());
        for (int i = 0; i < newDir.size(); i++)
        {
            Log.d("ACTIVITY DIR POINTS", "LATITUDE: " + df.format(newDir.get(i).latitude)
                    + ", LONGITUDE" + df.format(newDir.get(i).longitude));
        }

    }
}
