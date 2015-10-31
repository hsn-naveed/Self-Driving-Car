package undergradsplusplus.ug;

import android.app.Fragment;
import android.location.Location;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.MapFragment;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.android.gms.maps.model.PolylineOptions;

import org.w3c.dom.Document;

import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by PhiTran on 10/24/15.
 */
public class Map_Fragment extends Fragment implements View.OnClickListener{
    public double Latitude;
    public double Longitude;
    List<LatLng> points = new ArrayList<LatLng>();

    public static double cLat, cLong;

    private GoogleMap map;
    GoogleDirection gd;

    GoogleMap.OnMapClickListener listener = new GoogleMap.OnMapClickListener() {
        @Override
        public void onMapClick(LatLng latLng) {
            Log.d("hello", "Latitude:" + Latitude);
            Log.d("hello", "Longitude:" + Longitude);

            addPoints(latLng);
            addMarker(latLng);

            points.add(latLng);
        }
    };

    private GoogleMap.OnMyLocationChangeListener myLocationChangeListener = new GoogleMap.OnMyLocationChangeListener() {
        @Override
        public void onMyLocationChange(Location location) {
            LatLng loc = new LatLng(location.getLatitude(), location.getLongitude());

            cLat = loc.latitude;
            cLong = loc.longitude;
            Log.d("LOC", "" + loc.longitude + ", " + loc.latitude);
        }
    };

    public void addMarker(LatLng latLng)
    {
        map.addMarker(new MarkerOptions()
                .position(latLng)
                .title("I'M HERE"));
    }

    public void addPoints(LatLng latLng)
    {
        Latitude = latLng.latitude;
        Longitude = latLng.longitude;

        TextView text;

        DecimalFormat df = new DecimalFormat("#.######");


        text = (TextView) getActivity().findViewById(R.id.longitude1);
        text.setText("Longitude\n" + df.format(Longitude));

        text = (TextView) getActivity().findViewById(R.id.latitude1);
        text.setText("Latitude\n" + df.format(Latitude));

    }

    public void clearPoints(LatLng latLng)
    {
        Latitude = latLng.latitude;
        Longitude = latLng.longitude;

        TextView text;

        DecimalFormat df = new DecimalFormat("#.######");


        text = (TextView) getActivity().findViewById(R.id.longitude1);
        text.setText("Longitude\n" + df.format(Longitude));

        text = (TextView) getActivity().findViewById(R.id.latitude1);
        text.setText("Latitude\n" + df.format(Latitude));

    }


    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
//        return inflater.inflate(R.layout.map_layout, container, false);

        View v = inflater.inflate(R.layout.map_layout, container, false);

        map = ((MapFragment) getChildFragmentManager().findFragmentById(R.id.gmaps)).getMap();

        map.setOnMapClickListener(listener);
        map.setMyLocationEnabled(true);
        map.setOnMyLocationChangeListener(myLocationChangeListener);

        Button RESET = (Button) v.findViewById(R.id.reset_button);
        RESET.setOnClickListener(this);
        Button GO = (Button) v.findViewById(R.id.go_button);
        GO.setOnClickListener(this);

        LatLng start = new LatLng(37.335434, -121.881380);
        LatLng end = new LatLng(37.336506, -121.879081);

        gd = new GoogleDirection(getActivity());
        gd.setOnDirectionResponseListener(new GoogleDirection.OnDirectionResponseListener() {
            public void onResponse(String status, Document doc, GoogleDirection gd) {
                Toast.makeText(getActivity(), status, Toast.LENGTH_SHORT).show();

                gd.animateDirection(map, gd.getDirection(doc), GoogleDirection.SPEED_NORMAL
                        , true, false, true, false, null, false, true, new PolylineOptions().width(3));
            }
        });

        gd.request(start, end, GoogleDirection.MODE_WALKING);


        return v;
    }


    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);


        Log.d("phil", "in map");
    }

    @Override
    public void onClick(View v) {
        switch (v.getId())
        {
            case R.id.reset_button:
                map.clear();
                points.clear();
                LatLng latty = new LatLng(0, 0);
                clearPoints(latty);
                Log.d("RESET", "RESET");
                break;

            case R.id.go_button:
                Log.d("CURR", "" + cLong + ", " + cLat);
                break;

            default:
                break;
        }
    }


}
