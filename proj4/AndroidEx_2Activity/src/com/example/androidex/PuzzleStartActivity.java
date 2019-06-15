package com.example.androidex;

import java.util.Random;

import android.R.drawable;
import android.annotation.SuppressLint;
import android.app.ActionBar.LayoutParams;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.Toast;



public class PuzzleStartActivity extends Activity{
	
	Random r = new Random();
	int row, col, size;
	int blanki, blankj;
	int[][] puzzle = new int[5][5];
	OnClickListener puzzle_click;	
	
	
	
		@SuppressLint("NewApi")
		@Override
	protected void onCreate(Bundle savedInstanceState) {

		//set init layout
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_puzzle_start);
		LinearLayout linear = (LinearLayout)findViewById(R.id.container);	
		
		// set window w, h
		int W, H;	
		DisplayMetrics dm = getApplicationContext().getResources().getDisplayMetrics();
		W = dm.widthPixels;
		H = dm.heightPixels;
		
		
		// get row, col data from prev activity
		Intent intent = getIntent();
		row = intent.getExtras().getInt("row");
		col = intent.getExtras().getInt("col");
		size = row * col;

		
		Shuffle_value_and_setting();
		// data setting at puzzle

		
		
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
		int[] temp = new int[25];
		// get row and column data. setting size (row * col)
		
		for (int i = 0 ; i < size ; i++){
			temp[i] = i+1;
		}
		
		for (int i = 0 ; i < size ; i++){
			int tnum, ran = r.nextInt(size);
			tnum = temp[ran];
			temp[ran] =  temp[i];
			temp[i] = tnum;
		}		
		// shuffle data
		
		for (int i = 0 ; i < row ; i++){
			for (int j = 0 ; j < col ; j++){
				puzzle[i][j] = temp[i*(row+1) + j];
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
