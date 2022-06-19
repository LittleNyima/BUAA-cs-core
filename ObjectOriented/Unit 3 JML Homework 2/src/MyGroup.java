import com.oocourse.spec2.main.Group;
import com.oocourse.spec2.main.Person;

import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashMap;

public class MyGroup implements Group {

    private final int id;
    private final HashMap<Integer, Person> people;
    private final ArrayList<Person> peopleList;
    private BigInteger conflictSum;
    private int ageSum;

    private int relation;
    private int value;

    public MyGroup(int id) {
        this.id = id;
        this.people = new HashMap<>();
        this.peopleList = new ArrayList<>();
        this.conflictSum = BigInteger.ZERO;
        this.ageSum = 0;
        this.relation = 0;
        this.value = 0;
    }

    @Override
    public int getId() {
        return id;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof Group) {
            return ((Group) obj).getId() == id;
        } else {
            return false;
        }
    }

    @Override
    public void addPerson(Person person) {
        people.put(person.getId(), person);
        peopleList.add(person);
        conflictSum = conflictSum.xor(person.getCharacter());
        ageSum = ageSum + person.getAge();
        for (Person pers : peopleList) {
            if (pers.isLinked(person)) {
                if (pers.equals(person)) {
                    relation++;
                    value = value + pers.queryValue(person);
                } else {
                    relation = relation + 2;
                    value = value + pers.queryValue(person) * 2;
                }
            }
        }
    }

    @Override
    public boolean hasPerson(Person person) {
        return people.containsKey(person.getId());
    }

    @Override
    public int getRelationSum() {
        /*int ret = 0;
        for (Person person : peopleList) {
            for (Person pers : peopleList) {
                if (person.isLinked(pers)) {
                    ret++;
                }
            }
        }
        return ret;*/
        return relation;
    }

    @Override
    public int getValueSum() {
        /*int ret = 0;
        for (Person person : peopleList) {
            for (Person pers : peopleList) {
                if (person.isLinked(pers)) {
                    ret += person.queryValue(pers);
                }
            }
        }
        return ret;*/
        return value;
    }

    @Override
    public BigInteger getConflictSum() {
        return conflictSum;
    }

    @Override
    public int getAgeMean() {
        if (people.isEmpty()) {
            return 0;
        } else {
            return ageSum / people.size();
        }
    }

    @Override
    public int getAgeVar() {
        if (people.isEmpty()) {
            return 0;
        } else {
            int mean = getAgeMean();
            int result = 0;
            for (Person person : peopleList) {
                result = result + (person.getAge() - mean) * (person.getAge() - mean);
            }
            return result / people.size();
        }
    }

    public int size() {
        return people.size();
    }

    public void add2relation() {
        relation = relation + 2;
    }

    public void add2value(int value) {
        this.value = this.value + value + value;
    }
}
