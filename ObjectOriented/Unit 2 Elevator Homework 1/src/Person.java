import com.oocourse.elevator1.PersonRequest;

public class Person {

    private final int id;
    private final int from;
    private final int to;
    private final int direction;

    Person(PersonRequest request) {
        if (request == null) {
            this.id = 0;
            this.from = 0;
            this.to = 0;
            this.direction = 0;
        } else {
            this.id = request.getPersonId();
            this.from = request.getFromFloor();
            this.to = request.getToFloor();
            this.direction = (from > to) ? -1 : 1;
        }
    }

    public int getId() {
        return id;
    }

    public int getFrom() {
        return from;
    }

    public int getTo() {
        return to;
    }

    public int getDirection() {
        return direction;
    }

    public boolean isEmpty() {
        return id == 0 && from == 0 && to == 0 && direction == 0;
    }
}
