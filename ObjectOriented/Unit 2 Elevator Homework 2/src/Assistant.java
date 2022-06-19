public class Assistant {

    private volatile int floor;
    private volatile int direction;
    private volatile int destination;
    private volatile int status; // 0 = idle, 1 = occupied
    private volatile int load;
    private volatile int capacity;

    Assistant() {
        this.floor = 4;
        this.direction = 1;
        this.destination = 1;
        this.status = 0;
        this.load = 0;
        this.capacity = 7;
    }

    public void setFloor(int floor) {
        this.floor = floor;
    }

    public void setDirection(int direction) {
        this.direction = direction;
    }

    public void setDestination(int destination) {
        this.destination = destination;
    }

    public void setStatus(int status) {
        this.status = status;
    }

    public void setLoad(int load) {
        this.load = load;
    }

    public void setCapacity(int capacity) {
        this.capacity = capacity;
    }

    public int getFloor() {
        return floor;
    }

    public int getDirection() {
        return direction;
    }

    public int getDestination() {
        return destination;
    }

    public int getStatus() {
        return status;
    }

    public int getLoad() {
        return load;
    }

    public int getCapacity() {
        return capacity;
    }
}
