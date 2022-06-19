import com.oocourse.spec3.main.Person;

import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashMap;

public class MyPerson implements Person {

    private final int id;
    private final String name;
    private final BigInteger character;
    private final int age;
    private final HashMap<Integer, MyRelation> acquaintance;
    private final ArrayList<MyRelation> peopleList;

    public MyPerson(int id, String name, BigInteger character, int age) {
        this.id = id;
        this.name = name;
        this.character = character;
        this.age = age;
        this.acquaintance = new HashMap<>();
        this.peopleList = new ArrayList<>();
    }

    @Override
    public int hashCode() {
        return id;
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
        if (obj instanceof Person) {
            return ((Person) obj).getId() == id;
        } else {
            return false;
        }
    }

    @Override
    public boolean isLinked(Person person) {
        if (this.id == person.getId()) {
            return true;
        } else {
            return acquaintance.containsKey(person.getId());
        }
    }

    @Override
    public int queryValue(Person person) {
        if (!acquaintance.containsKey(person.getId())) {
            return 0;
        } else {
            return acquaintance.get(person.getId()).getValue();
        }
    }

    @Override
    public int getAcquaintanceSum() {
        return acquaintance.size();
    }

    @Override
    public int compareTo(Person p2) {
        return name.compareTo(p2.getName());
    }

    public void addRelation(Person person, int value) {
        MyRelation relation = new MyRelation(person, value);
        acquaintance.put(person.getId(), relation);
        peopleList.add(relation);
    }

    public ArrayList<MyRelation> getRelationList() {
        return peopleList;
    }
}
