package com.example.androidex;

import java.util.Timer;
import java.util.TimerTask;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

public class timer_serv extends Service{
	
	private int elapsedTime = 0;
	private Timer mTimer;
	private TimerTask mTask;


	private boolean isStop;
		
	
	Itimer_serv.Stub mbinder = new Itimer_serv.Stub(){
		@Override
		public int gettime() throws RemoteException{
			return elapsedTime;
		}
	};
	
	@Override
	public void onCreate(){
		super.onCreate();

		elapsedTime = 0;
		mTask = new TimerTask(){
			public void run(){
				elapsedTime += 1;
			}
		};

		mTimer = new Timer();
		mTimer.schedule(mTask, 0, 1000);
	}

	@Override
	public IBinder onBind(Intent intent) {
		return mbinder;
	}
	
	@Override
	public boolean onUnbind(Intent intent){
		isStop = true;
		return super.onUnbind(intent);
	}

	@Override
    public void onDestroy() 
    {
          super.onDestroy();
          isStop = true;
    }
    
}
