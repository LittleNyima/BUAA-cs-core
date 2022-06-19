import com.oocourse.elevator3.PersonRequest;

public class Person {

    private final int id;
    private final int departure;
    private final int transfer;
    private final int termination;
    private final int direction;
    private final boolean reenter;
    private final boolean empty;

    Person(PersonRequest request) {
        if (request == null) {
            this.id = 0;
            this.departure = 0;
            this.transfer = 0;
            this.termination = 0;
            this.direction = 0;
            this.reenter = false;
            this.empty = true;
        } else {
            this.id = request.getPersonId();
            if (request.getFromFloor() < 0) {
                this.departure = request.getFromFloor() + 4;
            } else {
                this.departure = request.getFromFloor() + 3;
            }
            if (request.getToFloor() < 0) {
                this.termination = request.getToFloor() + 4;
            } else {
                this.termination = request.getToFloor() + 3;
            }
            this.transfer = 0;
            this.direction = (this.departure < this.termination) ? 1 : -1;
            this.reenter = false;
            this.empty = false;
        }
    }

    Person(int id, int departure, int transfer, int termination) {
        this.id = id;
        this.departure = departure;
        this.transfer = transfer;
        this.termination = termination;
        this.direction = (this.departure < this.transfer) ? 1 : -1;
        this.reenter = true;
        this.empty = false;
    }

    @Override
    public String toString() {
        return "Person{" +
                "id=" + id +
                ", departure=" + departure +
                ", transfer=" + transfer +
                ", termination=" + termination +
                ", direction=" + direction +
                ", reenter=" + reenter +
                ", empty=" + empty +
                '}';
    }

    public int getId() {
        return id;
    }

    public int getDeparture() {
        return departure;
    }

    public int getTransfer() {
        return transfer;
    }

    public int getTermination() {
        return termination;
    }

    public int getDirection() {
        return direction;
    }

    public boolean isReenter() {
        return reenter;
    }

    public boolean isEmpty() {
        return empty;
    }

    public boolean needTransfer() {
        return this.transfer != this.termination;
    }
}
