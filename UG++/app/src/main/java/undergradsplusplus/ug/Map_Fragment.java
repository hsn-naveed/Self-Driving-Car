package undergradsplusplus.ug;

import android.app.Fragment;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.MapFragment;
import com.google.android.gms.maps.MapView;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.LatLng;

/**
 * Created by PhiTran on 10/24/15.
 */
public class Map_Fragment extends Fragment{
    double Latitude;
    double Longitude;

    private GoogleMap map;
    GoogleMap.OnMapClickListener listener = new GoogleMap.OnMapClickListener() {
        @Override
        public void onMapClick(LatLng latLng) {
            Latitude = latLng.latitude;
            Longitude = latLng.longitude;

            Log.d("hello", "Latitude:" + Latitude);
            Log.d("hello", "Longitude:" + Longitude);


            TextView text;

            text = (TextView) getActivity().findViewById(R.id.longitude1);
            text.setText("Longitude\n" + Longitude);

        }
    };
    LatLng point;

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
//        return inflater.inflate(R.layout.map_layout, container, false);

        View v = inflater.inflate(R.layout.map_layout, container, false);

//        mapView = (MapView) mapView.findViewById(R.id.gmaps);
//        mapView.onCreate(savedInstanceState);

        map = ((MapFragment) getChildFragmentManager().findFragmentById(R.id.gmaps)).getMap();

        map.setOnMapClickListener(listener);


        return v;
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);



        Log.d("phil", "in map");
    }

}
