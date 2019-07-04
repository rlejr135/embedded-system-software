/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Original file: /home/embe/20141602/embedded_system_software/proj4/20141602_proj4/src/com/example/androidex/Itimer_serv.aidl
 */
package com.example.androidex;
public interface Itimer_serv extends android.os.IInterface
{
/** Local-side IPC implementation stub class. */
public static abstract class Stub extends android.os.Binder implements com.example.androidex.Itimer_serv
{
private static final java.lang.String DESCRIPTOR = "com.example.androidex.Itimer_serv";
/** Construct the stub at attach it to the interface. */
public Stub()
{
this.attachInterface(this, DESCRIPTOR);
}
/**
 * Cast an IBinder object into an com.example.androidex.Itimer_serv interface,
 * generating a proxy if needed.
 */
public static com.example.androidex.Itimer_serv asInterface(android.os.IBinder obj)
{
if ((obj==null)) {
return null;
}
android.os.IInterface iin = obj.queryLocalInterface(DESCRIPTOR);
if (((iin!=null)&&(iin instanceof com.example.androidex.Itimer_serv))) {
return ((com.example.androidex.Itimer_serv)iin);
}
return new com.example.androidex.Itimer_serv.Stub.Proxy(obj);
}
@Override public android.os.IBinder asBinder()
{
return this;
}
@Override public boolean onTransact(int code, android.os.Parcel data, android.os.Parcel reply, int flags) throws android.os.RemoteException
{
switch (code)
{
case INTERFACE_TRANSACTION:
{
reply.writeString(DESCRIPTOR);
return true;
}
case TRANSACTION_gettime:
{
data.enforceInterface(DESCRIPTOR);
int _result = this.gettime();
reply.writeNoException();
reply.writeInt(_result);
return true;
}
}
return super.onTransact(code, data, reply, flags);
}
private static class Proxy implements com.example.androidex.Itimer_serv
{
private android.os.IBinder mRemote;
Proxy(android.os.IBinder remote)
{
mRemote = remote;
}
@Override public android.os.IBinder asBinder()
{
return mRemote;
}
public java.lang.String getInterfaceDescriptor()
{
return DESCRIPTOR;
}
@Override public int gettime() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
int _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_gettime, _data, _reply, 0);
_reply.readException();
_result = _reply.readInt();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
}
static final int TRANSACTION_gettime = (android.os.IBinder.FIRST_CALL_TRANSACTION + 0);
}
public int gettime() throws android.os.RemoteException;
}
