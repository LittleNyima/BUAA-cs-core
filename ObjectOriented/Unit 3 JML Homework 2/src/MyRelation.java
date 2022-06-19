import com.oocourse.spec2.main.Person;

public class MyRelation {

    private final Person person;
    private final int value;

    public MyRelation(Person person, int value) {
        this.person = person;
        this.value = value;
    }

    public Person getPerson() {
        return person;
    }

    public int getValue() {
        return value;
    }
}
