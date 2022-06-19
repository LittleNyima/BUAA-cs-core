import com.oocourse.TimableOutput;

public class ConcurrentOutput {

    /*public static synchronized long println(Object obj, PrintStream stream) {
        long ret =  TimableOutput.println(obj, stream);
        ConcurrentOutput.class.notifyAll();
        return ret;
    }*/

    public static synchronized long println(Object obj) {
        long ret =  TimableOutput.println(obj);
        ConcurrentOutput.class.notifyAll();
        return ret;
    }

    /*public static synchronized long println(int i, PrintStream stream) {
        long ret = TimableOutput.println(i, stream);
        ConcurrentOutput.class.notifyAll();
        return ret;
    }

    public static synchronized long println(int i) {
        long ret = TimableOutput.println(i);
        ConcurrentOutput.class.notifyAll();
        return ret;
    }

    public static synchronized long println(boolean b, PrintStream stream) {
        long ret = TimableOutput.println(b, stream);
        ConcurrentOutput.class.notifyAll();
        return ret;
    }

    public static synchronized long println(boolean b) {
        long ret = TimableOutput.println(b);
        ConcurrentOutput.class.notifyAll();
        return ret;
    }

    public static synchronized long println(char c, PrintStream stream) {
        long ret = TimableOutput.println(c, stream);
        ConcurrentOutput.class.notifyAll();
        return ret;
    }

    public static synchronized long println(char c) {
        long ret = TimableOutput.println(c);
        ConcurrentOutput.class.notifyAll();
        return ret;
    }

    public static synchronized long println(long l, PrintStream stream) {
        long ret = TimableOutput.println(l, stream);
        ConcurrentOutput.class.notifyAll();
        return ret;
    }

    public static synchronized long println(long l) {
        long ret = TimableOutput.println(l);
        ConcurrentOutput.class.notifyAll();
        return ret;
    }

    public static synchronized long println(float f, PrintStream stream) {
        long ret = TimableOutput.println(f, stream);
        ConcurrentOutput.class.notifyAll();
        return ret;
    }

    public static synchronized long println(float f) {
        long ret = TimableOutput.println(f);
        ConcurrentOutput.class.notifyAll();
        return ret;
    }

    public static synchronized long println(char[] s, PrintStream stream) {
        long ret = TimableOutput.println(s, stream);
        ConcurrentOutput.class.notifyAll();
        return ret;
    }

    public static synchronized long println(char[] s) {
        long ret = TimableOutput.println(s);
        ConcurrentOutput.class.notifyAll();
        return ret;
    }

    public static synchronized long println(double d, PrintStream stream) {
        long ret = TimableOutput.println(d, stream);
        ConcurrentOutput.class.notifyAll();
        return ret;
    }

    public static synchronized long println(double d) {
        long ret = TimableOutput.println(d);
        ConcurrentOutput.class.notifyAll();
        return ret;
    }*/
}
