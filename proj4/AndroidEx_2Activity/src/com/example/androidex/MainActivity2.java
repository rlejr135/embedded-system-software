package com.example.androidex;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;



public class MainActivity2 extends Activity{

		LinearLayout linear;
		EditText data;
		TextView tv;
		
		int row, col;
		@Override		
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main2);
		linear = (LinearLayout)findViewById(R.id.container);
		
		
		data = (EditText)findViewById(R.id.editText1);
		tv=(TextView)findViewById(R.id.textView1);
		Button btn=(Button)findViewById(R.id.Make_Buttons);
		OnClickListener listener=new OnClickListener(){
			public void onClick(View v){
//				Intent intent=new Intent(MainActivity2.this, PuzzleStartActivity.class);
//				startActivity(intent);
				String temp=data.getText().toString();
				
				row = temp.charAt(0);
				col = temp.charAt(2);
				row = row - 48;
				col = col - 48;
				tv.setText(String.valueOf(row));
			}
		};
		btn.setOnClickListener(listener);
	}

}

/*
public class MainActivity extends Activity{
	
	LinearLayout linear;
	EditText data;
	TextView tv;
	Button btn;
	OnClickListener ltn;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		linear = (LinearLayout)findViewById(R.id.container);
		
		data=(EditText)findViewById(R.id.editText1);
		tv=(TextView)findViewById(R.id.textView1);
		Button btn=(Button)findViewById(R.id.button1);
		ltn=new OnClickListener(){
			public void onClick(View v){
				String temp=data.getText().toString();
				tv.setText(temp);
			}
		};
		btn.setOnClickListener(ltn);
		
	}
}

*/
