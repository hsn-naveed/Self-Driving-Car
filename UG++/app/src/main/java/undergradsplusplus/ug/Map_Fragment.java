package undergradsplusplus.ug;

import android.app.Fragment;
import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.MapFragment;
import com.google.android.gms.maps.MapView;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.android.gms.maps.model.Polyline;
import com.google.android.gms.maps.model.PolylineOptions;

import java.text.DecimalFormat;
import java.util.ArrayList;

/**
 * Created by PhiTran on 10/24/15.
 */
public class Map_Fragment extends Fragment implements View.OnClickListener{
    public double Latitude;
    public double Longitude;

    private GoogleMap map;
    GoogleMap.OnMapClickListener listener = new GoogleMap.OnMapClickListener() {
        @Override
        public void onMapClick(LatLng latLng) {
            Latitude = latLng.latitude;
            Longitude = latLng.longitude;

            Log.d("hello", "Latitude:" + Latitude);
            Log.d("hello", "Longitude:" + Longitude);

            TextView text;

            DecimalFormat df = new DecimalFormat("#.######");

            text = (TextView) getActivity().findViewById(R.id.longitude1);
            text.setText("Longitude\n" + df.format(Longitude));

            text = (TextView) getActivity().findViewById(R.id.latitude1);
            text.setText("Latitude\n" + df.format(Latitude));


            addMarker(latLng);


        }
    };

    public void addMarker(LatLng latLng)
    {
        map.addMarker(new MarkerOptions()
                .position(latLng)
                .title("I'M HERE"));
    }



    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
//        return inflater.inflate(R.layout.map_layout, container, false);

        View v = inflater.inflate(R.layout.map_layout, container, false);

        map = ((MapFragment) getChildFragmentManager().findFragmentById(R.id.gmaps)).getMap();

        map.setOnMapClickListener(listener);

        Button RESET = (Button) v.findViewById(R.id.reset_button);
        RESET.setOnClickListener(this);

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
                Latitude = 0;
                Longitude = 0;
                break;
        }
    }
}
