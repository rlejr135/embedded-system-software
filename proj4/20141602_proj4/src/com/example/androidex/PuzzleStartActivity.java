package com.example.androidex;

import java.util.Random;

import android.R.drawable;
import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.app.ActionBar.LayoutParams;
import android.app.Activity;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;


public class PuzzleStartActivity extends Activity{
	
	Random r = new Random();
	int row, col, size;
	int blanki, blankj;
	int[][] puzzle = new int[6][6];
	int[][] dir = {{0, 0, 1, -1}, {1, -1, 0, 0}};
	
	
	private EditText data;	
	private LinearLayout linear;
	private TextView tv;
	private Itimer_serv binder;
	
	
	// for binder 
	private ServiceConnection connection = new ServiceConnection(){
		@Override
		public void onServiceConnected(ComponentName name, IBinder service){
			binder = Itimer_serv.Stub.asInterface(service);
		}
		
		@Override
		public void onServiceDisconnected(ComponentName name){
			
		}
	};
	private Intent bindintent;
	private boolean running = true;

	
	@Override
	protected void onCreate(Bundle savedInstanceState) {

		//set init layout
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_puzzle_start);	
		linear = (LinearLayout)findViewById(R.id.container);	
	
		data = (EditText)findViewById(R.id.editText1);
		
		Button edit_btn=(Button)findViewById(R.id.Make_Buttons);
		
		// when created and click start puzzle, row, col input and make
		OnClickListener listener=new OnClickListener(){
			public void onClick(View v){
				String temp=data.getText().toString();
				row = temp.charAt(0);
				col = temp.charAt(2);
				row = row - 48;
				col = col - 48;
			
				// check row and col
				if (row > 5 || col > 5){
					Toast.makeText(getApplicationContext(), "please input under 6", Toast.LENGTH_LONG).show();
					return;
				}
	
				if (row < 1 || col < 1){
					Toast.makeText(getApplicationContext(), "please input over 0", Toast.LENGTH_LONG).show();
					return;
				}
				// start timer service			
				bindintent = new Intent(PuzzleStartActivity.this, timer_serv.class);
				bindService(bindintent, connection, BIND_AUTO_CREATE);		


				running = true;			
				new Thread(new gettimeThread()).start();
				
				
				// make button dynamic
				visible_button_and_start();
			}
		};
		edit_btn.setOnClickListener(listener);
	}
	

	
	// fail, why Ui is not change?
	private class gettimeThread implements Runnable{
		private Handler handler = new Handler();
		
		@Override
		public void run(){

			while(running){
				if(binder == null)
					continue;
				
				handler.post(new Runnable(){
					@Override
					public void run(){
						tv = (TextView)findViewById(R.id.textView11);
						try{
							String min = String.format("%02d",binder.gettime() / 60);
							String sec = String.format("%02d",binder.gettime() % 60);
							tv.setText(min + ":" + sec);
							
						}catch(RemoteException e){
							e.printStackTrace();
						}
					}
				});
				
				try{
					Thread.sleep(500);					
				}catch(InterruptedException e){
					e.printStackTrace();
				}
			}
		}
	}

	
	
		
		
	
	// make button dynamic
	@TargetApi(Build.VERSION_CODES.HONEYCOMB)
	@SuppressLint("NewApi")
	protected void visible_button_and_start(){		
		tv = (TextView)findViewById(R.id.textView11);
		setContentView(R.layout.activity_puzzle_start);
		linear = (LinearLayout)findViewById(R.id.container);	
		
		
		
		// get screen size
		int W, H;	
		DisplayMetrics dm = getApplicationContext().getResources().getDisplayMetrics();
		W = dm.widthPixels;
		H = dm.heightPixels;		
		
		size = row * col;

	
		// setting puzzle data
		Shuffle_value_and_setting();

		// If puzzle size is 1*1, always finish immediately.
		check_finish();
		

		// make row * col button
		for (int i = 0 ; i < row ; i++){
			LinearLayout rowLayout = new LinearLayout(this);
			rowLayout.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT));

			for(int j = 0 ; j < col ; j++){

				Button btn = new Button(this);            
	            btn.setLayoutParams(new LayoutParams((W / col) ,  ((H-200) / row)));
	            btn.setText(""+ puzzle[i][j]); 
	            btn.setId((int)puzzle[i][j]);

	            if (puzzle[i][j] == size){	// it is blank
	            	btn.setBackgroundColor(Color.BLACK);
	            	blanki = i;
	            	blankj = j;
	            }
	            else{						// it is not blank.
	            	OnClickListener puzzle_click = new OnClickListener(){
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
	
	

	// shuffle array. and give each button
	protected void Shuffle_value_and_setting() {

		// set init puzzle value
		for (int i = 0 ; i < row ; i++){
			for (int j = 0 ; j < col ; j++){
				puzzle[i][j] = i*col + j + 1;
			}
		}
		boolean flag = true;
		while (flag){
			int tmp;
			int previ = row-1, prevj = col-1;
			int nexti, nextj;	
			
			// shuffle puzzle value
			for (int i = 0 ; i < size*4 ; i++){
				tmp = r.nextInt(4);
				
				nexti = previ + dir[0][tmp];
				nextj = prevj + dir[1][tmp];
				
				// Find blank area and randomly select an adjacent button.
				// if selected adjacent button's position is in puzzle range, change both
				if (nexti >= 0 && nexti< row && nextj >= 0 && nextj < col){
					int tnum = puzzle[nexti][nextj];
					puzzle[nexti][nextj] = puzzle[previ][prevj];
					puzzle[previ][prevj] = tnum;
					
					previ = nexti;
					prevj = nextj;
				}
			}
			
			// finish shuffle but it is same with solve puzzle, shuffle again
			
			int num = 1;
			// check puzzle's id (puzzle text) is successive
			for (int i = 0 ; i < row ; i++){
				for (int j = 0 ; j < col ; j++){
					if (puzzle[i][j] != num){
						flag = false;
					}
					num++;
				}
			}
			
		}
	}

	
	
	
	// when button is pressed, change pushed button and black button
	// also change button id, and where button is.
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
		
		// check pushed button and blank is adjacent
		int q;
		for (q = 0 ; q < 4 ; q++){
			if (i+dir[0][q] == blanki && j + dir[1][q] == blankj){
				break;
			}
		}
		// if not, do nothing
		if (q == 4){
			return;
		}

		
		
		// change puzzle value
		int tmp = puzzle[i][j];
		puzzle[i][j] = puzzle[blanki][blankj];
		puzzle[blanki][blankj] = tmp;
		
		
		// find now button's attribute
		Button btn = (Button)findViewById(v.getId());	
        btn.setText(""+ puzzle[i][j]); 
    	btn.setBackgroundColor(Color.BLACK);       
        btn.setOnClickListener(null);
        btn.setId(tmpId);       
        
        
        // find blank button's attribute
        Button btn_2 = (Button)findViewById(size);
        btn_2.setText("" + puzzle[blanki][blankj]);
        btn_2.setBackgroundResource(drawable.btn_default);
    	OnClickListener puzzle_click = new OnClickListener(){
			public void onClick(View v) {
				change_puzzle(v);
				check_finish();
			}
    	};
        btn_2.setOnClickListener(puzzle_click);
        btn_2.setId(cId);
        
        
        btn.setId(size);
        
        // set new blank button position
        blanki = i;
        blankj = j;
               
	}
	
	// if finish, message to user and go back first screen
	protected void check_finish(){
		int flag = 0;	
		int i, j, num = 1;
		// check puzzle's id (puzzle text) is successive
		for (i = 0 ; i < row ; i++){
			for (j = 0 ; j < col ; j++){
				if (puzzle[i][j] != num){
					flag = 1;
				}
				num++;
			}
		}

		if (flag == 0){		
			
			// unbind timer service
			unbindService(connection);
			running = false;
			
			// print game finish message and go back main activity
			Toast.makeText(getApplicationContext(), "game finish!", Toast.LENGTH_LONG).show();
			Intent intent=new Intent(PuzzleStartActivity.this, MainActivity.class);
			startActivity(intent);
		}
	}	
}


