import com.oocourse.spec1.main.Person;

import java.math.BigInteger;
import java.util.ArrayList;

public class MyPerson implements Person {

    private int id;
    private String name;
    private BigInteger character;
    private int age;
    private ArrayList<Person> acquaintance;
    private ArrayList<Integer> value;

    public MyPerson(int id, String name, BigInteger character, int age) {
        this.id = id;
        this.name = name;
        this.character = character;
        this.age = age;
        this.acquaintance = new ArrayList<>();
        this.value = new ArrayList<>();
    }

    @Override
    public int getId() {
        return id;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public BigInteger getCharacter() {
        return character;
    }

    @Override
    public int getAge() {
        return age;
    }

    @Override
    public boolean equals(Object obj) {
        if (!(obj instanceof Person)) {
            return false;
        } else {
            return ((Person) obj).getId() == id;
        }
    }

    @Override
    public boolean isLinked(Person person) {
        if (person.getId() == id) {
            return true;
        } else {
            for (Person acq : acquaintance) {
                if (acq.getId() == person.getId()) {
                    return true;
                }
            }
            return false;
        }
    }

    @Override
    public int queryValue(Person person) {
        for (int i = 0; i < acquaintance.size(); i++) {
            if (acquaintance.get(i).getId() == person.getId()) {
                return value.get(i);
            }
        }
        return 0;
    }

    @Override
    public int getAcquaintanceSum() {
        return acquaintance.size();
    }

    @Override
    public int compareTo(Person p2) {
        return name.compareTo(p2.getName());
    }

    public ArrayList<Person> getAcquaintance() {
        return acquaintance;
    }

    public void addRelation(Person person, int value) {
        this.acquaintance.add(person);
        this.value.add(value);
    }
}
