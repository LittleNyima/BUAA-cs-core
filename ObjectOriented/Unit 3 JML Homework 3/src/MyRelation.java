import com.oocourse.spec3.main.Person;

public class MyRelation implements Comparable<MyRelation> {

    private final Person person;
    private final int value;

    public MyRelation(Person person, int value) {
        this.person = person;
        this.value = value;
    }

    @Override
    public int compareTo(MyRelation o) {
        return this.value - o.value;
    }

    public Person getPerson() {
        return this.person;
    }

    public int getValue() {
        return this.value;
    }
}
