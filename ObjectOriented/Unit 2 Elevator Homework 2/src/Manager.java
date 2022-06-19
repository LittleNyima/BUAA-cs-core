import java.util.ArrayList;

public class Manager {

    private static int elevatorNumber;
    private static ArrayList<WaitQueue> queues;
    private static ArrayList<Assistant> assistants;
    /*
     * Average distribution
     */
    private static int inTurn;

    private Manager() {
    }

    public static void init(int number) {
        elevatorNumber = number;
        queues = new ArrayList<>();
        assistants = new ArrayList<>();
        inTurn = 0;
    }

    public static void setUpPair(WaitQueue queue, Assistant assistant) {
        queues.add(queue);
        assistants.add(assistant);
    }

    public static void distribute(Person person) {
        if (person.isEmpty()) {
            for (int i = 0; i < elevatorNumber; i++) {
                queues.get(i).enqueue(person);
            }
            return;
        }
        inTurn = (inTurn + 1) % elevatorNumber;
        queues.get(inTurn).enqueue(person);
    }
}
