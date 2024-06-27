package com.example.proyecto_vison;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

import java.io.IOException;
import java.io.InputStream;
import java.util.List;

public class ImagePickerActivity extends AppCompatActivity {

    private static final int REQUEST_IMAGE_CAPTURE = 1;
    private static final int REQUEST_SELECT_IMAGE = 2;
    private static final int REQUEST_CAMERA_PERMISSION = 100;
    private static final String TAG = "ImagePickerActivity";

    private ImageView imageView;
    private ImageView imageViewGrayscale;

    static {
        System.loadLibrary("proyecto_vison");
    }

    public native void procesarParte2(long matAddrRgba, long matAddrRes);
    public native void procesarImagenesLBP(AssetManager assetManager);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_image_picker);

        imageView = findViewById(R.id.image_view);
        imageViewGrayscale = findViewById(R.id.image_view_grayscale);
        Button btnTakePhoto = findViewById(R.id.btn_take_photo);
        Button btnSelectPhoto = findViewById(R.id.btn_select_photo);
        Button btnProcesarImagenesLBP = findViewById(R.id.btn_procesar_imagenes_lbp);

        btnTakePhoto.setOnClickListener(v -> {
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA}, REQUEST_CAMERA_PERMISSION);
            } else {
                takePhoto();
            }
        });

        btnSelectPhoto.setOnClickListener(v -> selectPhoto());

        btnProcesarImagenesLBP.setOnClickListener(v -> {
            Toast.makeText(this, "Procesando imágenes LBP...", Toast.LENGTH_SHORT).show();
            new Thread(() -> {
                procesarImagenesLBP(getAssets());
                runOnUiThread(() -> {
                    Toast.makeText(this, "Procesamiento de imágenes LBP completado.", Toast.LENGTH_SHORT).show();
                });
            }).start();
        });
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == REQUEST_CAMERA_PERMISSION) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                takePhoto();
            } else {
                Toast.makeText(this, "Camera permission is required to take photo", Toast.LENGTH_LONG).show();
            }
        }
    }

    private void takePhoto() {
        Log.d(TAG, "Attempting to take a photo");
        Intent takePictureIntent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
        PackageManager packageManager = getPackageManager();
        List<ResolveInfo> activities = packageManager.queryIntentActivities(takePictureIntent, PackageManager.MATCH_DEFAULT_ONLY);

        if (activities.size() > 0) {
            ResolveInfo resolved = activities.get(0); // Toma la primera aplicación de cámara disponible
            takePictureIntent.setClassName(resolved.activityInfo.packageName, resolved.activityInfo.name);
            Log.d(TAG, "Using camera app: " + resolved.activityInfo.packageName);
            startActivityForResult(takePictureIntent, REQUEST_IMAGE_CAPTURE);
        } else {
            Log.e(TAG, "No camera app found");
            Toast.makeText(this, "No camera app found", Toast.LENGTH_SHORT).show();
        }
    }

    private void selectPhoto() {
        Log.d(TAG, "Selecting photo from gallery");
        Intent selectPictureIntent = new Intent(Intent.ACTION_PICK, MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
        startActivityForResult(selectPictureIntent, REQUEST_SELECT_IMAGE);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode == RESULT_OK) {
            if (requestCode == REQUEST_IMAGE_CAPTURE && data != null) {
                Bundle extras = data.getExtras();
                Bitmap imageBitmap = (Bitmap) extras.get("data");
                imageView.setImageBitmap(imageBitmap);
                enviarImagenAProcesar(imageBitmap);
            } else if (requestCode == REQUEST_SELECT_IMAGE && data != null) {
                Uri imageUri = data.getData();
                try {
                    InputStream imageStream = getContentResolver().openInputStream(imageUri);
                    Bitmap selectedImage = BitmapFactory.decodeStream(imageStream);
                    imageView.setImageBitmap(selectedImage);
                    enviarImagenAProcesar(selectedImage);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    private void enviarImagenAProcesar(Bitmap bitmapOriginal) {
        Mat matOriginal = new Mat();
        Utils.bitmapToMat(bitmapOriginal, matOriginal);
        Mat matRes = new Mat(matOriginal.rows(), matOriginal.cols(), CvType.CV_8UC1);
        procesarParte2(matOriginal.getNativeObjAddr(), matRes.getNativeObjAddr());
        Bitmap bitmapRes = Bitmap.createBitmap(matRes.cols(), matRes.rows(), Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(matRes, bitmapRes);
        imageViewGrayscale.setImageBitmap(bitmapRes);
    }
}
