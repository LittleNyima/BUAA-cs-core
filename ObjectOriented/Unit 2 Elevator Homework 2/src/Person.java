import com.oocourse.elevator2.PersonRequest;

public class Person {

    private final int id;
    private final int from;
    private final int to;
    private final int direction;
    private final boolean empty;

    Person(PersonRequest request) {
        if (request == null) {
            this.id = 0;
            this.from = 0;
            this.to = 0;
            this.direction = 0;
            this.empty = true;
        } else {
            this.id = request.getPersonId();
            if (request.getFromFloor() < 0) {
                this.from = request.getFromFloor() + 4;
            } else {
                this.from = request.getFromFloor() + 3;
            }
            if (request.getToFloor() < 0) {
                this.to = request.getToFloor() + 4;
            } else {
                this.to = request.getToFloor() + 3;
            }
            this.direction = (this.from < this.to) ? 1 : -1;
            this.empty = false;
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
        return empty;
    }
}
