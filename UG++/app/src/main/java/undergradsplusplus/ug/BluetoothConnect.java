
package undergradsplusplus.ug;

/**
 * Created by ming on 11/10/15.
 */
public class BluetoothConnect extends Thread {

}
//package undergradsplusplus.ug;
//
//import android.bluetooth.BluetoothDevice;
//import android.bluetooth.BluetoothSocket;
//
//import java.io.IOException;
//import java.util.UUID;
//
///**
// * Created by PhiTran on 11/3/15.
// * Creating
// */
//public class BluetoothConnect extends Thread{
////UUID 71e24236-1cb1-4c53-9217-93933abe4680
//    private final BluetoothSocket mmSocket;
//    private final BluetoothDevice mmDevice;
//
//    private final static UUID UG_UUID = UUID.fromString("71e24236-1cb1-4c53-9217-93933abe4680");
//
//    public BluetoothConnect (BluetoothDevice device)
//    {
//        BluetoothSocket tmp = null;
//        mmDevice = device;
//        try
//        {
//            tmp = device.createInsecureRfcommSocketToServiceRecord(UG_UUID);
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
//
//        mmSocket = tmp;
//    }
//
//    public void run ()
//    {
//        Setup_Fragment.mBluetoothAdapter;
//    }
//
//}
