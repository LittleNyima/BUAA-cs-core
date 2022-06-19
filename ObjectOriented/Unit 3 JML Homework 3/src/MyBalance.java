import com.oocourse.spec3.main.Person;

public class MyBalance {

    private final Person person;
    private int money;

    public MyBalance(Person person, int money) {
        this.person = person;
        this.money = money;
    }

    public Person getPerson() {
        return this.person;
    }

    public int getMoney() {
        return this.money;
    }

    public void saveMoney(int value) {
        this.money = this.money + value;
    }

    public void withdraw(int value) {
        this.money = this.money - value;
    }
}
