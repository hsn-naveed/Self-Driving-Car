package undergradsplusplus.ug;


import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.support.v4.app.FragmentActivity;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

/*
*      TODO 1:       Implement Setup, Map, and then Status Fragments
*      TODO 1.2:     Establish Bluetooth communication
*      TODO 2:       Implement cross-fragment interaction
*      TODO 2.2:     Google Maps capabilities
*      TODO 3:       Establish CAN communication
*      TODO 4:       Parse CAN ID's
*/

public class MainActivity extends AppCompatActivity {

    public final static String EXTRA_MESSAGE = "ug.undergradsplusplus.MESSAGE";

    FragmentManager manager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        manager = getFragmentManager();
        Log.d("onMAIN", "IN MAIN NOW");
    }

    public void setupButton(View v)
    {
        Setup_Fragment setupFrag = new Setup_Fragment();
        FragmentTransaction transaction = manager.beginTransaction();
        transaction.add(R.id.fragment_layout, setupFrag, "SETUP");
        transaction.commit();
    }

    public void mapButton(View v)
    {
        Map_Fragment mapFrag = new Map_Fragment();
        FragmentTransaction transaction = manager.beginTransaction();
        transaction.add(R.id.fragment_layout, mapFrag, "MAP");
        transaction.commit();
    }

    public void statusButton(View v)
    {
        Status_Fragment statusFrag = new Status_Fragment();
        FragmentTransaction transaction = manager.beginTransaction();
        transaction.add(R.id.fragment_layout, statusFrag, "STATUS");
        transaction.commit();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
