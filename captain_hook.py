import frida
import sys

def on_message(message, data):
    print("[on_message] message:", message, "data:", data)

pid = frida.spawn("main", ["main", "test.sqlite3", "bob"]) # Start the program, but paused
session = frida.attach(pid) # Hook into the program

script = session.create_script("""
Interceptor.attach(Module.getExportByName("libsqlite3.so.0.8.6", "sqlite3_prepare_v2"), {
    onEnter(args) { send(args[1].readCString()); }
});
Interceptor.attach(Module.getExportByName("libsqlite3.so.0.8.6", "sqlite3_bind_text"), {
    onEnter(args) { send(args[2].readCString()); }
});
Interceptor.attach(Module.getExportByName("libsqlite3.so.0.8.6", "sqlite3_step"), {
    onEnter(args) { send("RUNNING STATEMENT!"); }
});

""")
script.on("message", on_message)
script.load()

frida.resume(pid) # Run the program

sys.stdin.read()  # IMPORTANT: Otherwise the script terminates before the messages are received