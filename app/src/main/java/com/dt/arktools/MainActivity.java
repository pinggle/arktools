package com.dt.arktools;

import android.os.Bundle;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import com.dt.arktools.databinding.ActivityMainBinding;
import com.dt.arktools.utils.ArkUtils;

public class MainActivity extends AppCompatActivity {


    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        ArkUtils.init();

        // Example of a call to a native method
        TextView tv = binding.sampleText;
        tv.setText(ArkUtils.stringFromJNI());

        //ArkUtils.dexDump("/sdcard/ark/classes.dex", "d");
        ArkUtils.javaSayHi();
    }


}