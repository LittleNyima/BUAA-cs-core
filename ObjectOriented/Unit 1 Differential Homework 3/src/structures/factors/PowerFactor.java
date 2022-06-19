package structures.factors;

import interfaces.FactorType;

import java.math.BigInteger;
import java.util.ArrayList;

public class PowerFactor implements FactorType {

    private BigInteger index;

    public PowerFactor(BigInteger index) {
        this.index = new BigInteger(index.toString());
    }

    public PowerFactor(String index) {
        this.index = new BigInteger(index);
    }

    @Override
    public String toString() {
        if (index.compareTo(BigInteger.ZERO) == 0) {
            return "1";
        } else if (index.compareTo(BigInteger.ONE) != 0) {
            return "x**" + index.toString();
        } else {
            return "x";
        }
    }

    @Override
    public boolean withSign() {
        return this.toString().charAt(0) == '-' || this.toString().charAt(0) == '+';
    }

    @Override
    public boolean isEmpty() {
        return this.index.compareTo(BigInteger.ZERO) == 0;
    }

    @Override
    public boolean canMerge(FactorType another) {
        return another instanceof PowerFactor;
    }

    @Override
    public void mulUpdate(FactorType another) {
        if (another instanceof PowerFactor) {
            this.index = this.index.add(((PowerFactor) another).index);
        }
    }

    @Override
    public boolean equals(Object another) {
        return another instanceof PowerFactor
                && this.index.compareTo(((PowerFactor) another).index) == 0;
    }

    @Override
    public ArrayList<FactorType> differential() {
        ArrayList<FactorType> ret = new ArrayList<>();
        if (this.index.compareTo(BigInteger.ONE) != 0) {
            ret.add(new PowerFactor(this.index.subtract(BigInteger.ONE)));
        }
        ret.add(new ConstFactor(this.index));
        return ret;
    }
}
