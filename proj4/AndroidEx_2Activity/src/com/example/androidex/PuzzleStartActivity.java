package com.example.androidex;

import java.util.Random;

import android.R.drawable;
import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.app.ActionBar.LayoutParams;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Toast;



public class PuzzleStartActivity extends Activity{
	
	Random r = new Random();
	int row, col, size;
	int blanki, blankj;
	int[][] puzzle = new int[6][6];
	OnClickListener puzzle_click;	
	
	EditText data;
	
	LinearLayout linear;
	
		@Override
	protected void onCreate(Bundle savedInstanceState) {

		//set init layout
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_puzzle_start);
		linear = (LinearLayout)findViewById(R.id.container);	
		

		data = (EditText)findViewById(R.id.editText1);
		Button edit_btn=(Button)findViewById(R.id.Make_Buttons);
		OnClickListener listener=new OnClickListener(){
			public void onClick(View v){
				String temp=data.getText().toString();
				row = temp.charAt(0);
				col = temp.charAt(2);
				row = row - 48;
				col = col - 48;
				
				if (row > 5 || col > 5){
					Toast.makeText(getApplicationContext(), "please input under 5", Toast.LENGTH_LONG).show();
					return;
				}
				
				visible_button_and_start();
			}
		};
		edit_btn.setOnClickListener(listener);
	}
		
	
	@TargetApi(Build.VERSION_CODES.HONEYCOMB)
	@SuppressLint("NewApi")
	protected void visible_button_and_start(){		
		// set window w, h

		setContentView(R.layout.activity_puzzle_start);
		linear = (LinearLayout)findViewById(R.id.container);	
		
		
		int W, H;	
		DisplayMetrics dm = getApplicationContext().getResources().getDisplayMetrics();
		W = dm.widthPixels;
		H = dm.heightPixels;		
		
		size = row * col;

		
		Shuffle_value_and_setting();
		// data setting at puzzle
	
		check_finish();
		
		// make row * col button
		for (int i = 0 ; i < row ; i++){
			LinearLayout rowLayout = new LinearLayout(this);

			rowLayout.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT));

			for(int j = 0 ; j < col ; j++){

				Button btn = new Button(this);            
	            btn.setLayoutParams(new LayoutParams((W / col) ,  ((H-150) / row)));
	            btn.setText(""+ puzzle[i][j]); 
	            btn.setId((int)puzzle[i][j]);

	            if (puzzle[i][j] == size){	// it is blank
	            	btn.setBackgroundColor(Color.BLACK);
	            	blanki = i;
	            	blankj = j;
	            }
	            else{
	            	puzzle_click = new OnClickListener(){
						public void onClick(View v) {
							change_puzzle(v);
							check_finish();
						}
	            	};
		            btn.setOnClickListener(puzzle_click);
	            }

				rowLayout.addView(btn);
			}
			linear.addView(rowLayout);
		}
	}
	
	

	protected void Shuffle_value_and_setting() {
		int[] temp = new int[size];
		// get row and column data. setting size (row * col)
		
		for (int i = 0 ; i < size ; i++){
			temp[i] = i+1;

		}
		
		for (int i = 0 ; i < size ; i++){
			int tnum, ran = r.nextInt(size);
			if (ran >= size || ran < 0) continue;
			tnum = temp[ran];
			temp[ran] =  temp[i];
			temp[i] = tnum;
		}		
		// shuffle data
		
		for (int i = 0 ; i < row ; i++){
			for (int j = 0 ; j < col ; j++){
				puzzle[i][j] = temp[i*col + j];	
			}
		}

	}
	
	
	
	protected void change_puzzle(View v){
	
		int i = 0, j = 0;
		int cId = v.getId(), tmpId = 9999;	
		for (int k = 0 ; k < row ; k++){
			for (int q = 0 ; q < col ; q++){
				if (cId == puzzle[k][q]){
					i = k;
					j = q;
					break;
				}
			}
		}
		

		
		int tmp = puzzle[i][j];
		puzzle[i][j] = puzzle[blanki][blankj];
		puzzle[blanki][blankj] = tmp;
		
		
		Button btn = (Button)findViewById(v.getId());	
        btn.setText(""+ puzzle[i][j]); 
    	btn.setBackgroundColor(Color.BLACK);       
        btn.setOnClickListener(null);
        btn.setId(tmpId);       
        
        
        btn = (Button)findViewById(size);
        btn.setText("" + puzzle[blanki][blankj]);
        btn.setBackgroundResource(drawable.btn_default);
    	puzzle_click = new OnClickListener(){
			public void onClick(View v) {
				change_puzzle(v);
				check_finish();
			}
    	};
        btn.setOnClickListener(puzzle_click);
        btn.setId(cId);
        
        
        btn = (Button)findViewById(tmpId);
        btn.setId(size);
        
        blanki = i;
        blankj = j;
        
        
	}
	
	protected void check_finish(){
		int flag = 0;
		
		int i, j, num = 1;
		for (i = 0 ; i < row ; i++){
			for (j = 0 ; j < col ; j++){
				if (puzzle[i][j] != num){
					flag = 1;
				}
				num++;
			}
		}
		Log.d("flag", String.valueOf(flag));	
		if (flag == 0){		
			Toast.makeText(getApplicationContext(), "game finish!", Toast.LENGTH_LONG).show();
			Intent intent=new Intent(PuzzleStartActivity.this, MainActivity.class);
			startActivity(intent);
		}
	}
	
	
}
