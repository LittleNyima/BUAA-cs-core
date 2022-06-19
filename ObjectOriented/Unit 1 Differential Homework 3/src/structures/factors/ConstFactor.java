package structures.factors;

import interfaces.FactorType;

import java.math.BigInteger;
import java.util.ArrayList;

public class ConstFactor implements FactorType {

    private BigInteger value;

    public ConstFactor(BigInteger value) {
        this.value = new BigInteger(value.toString());
    }

    public ConstFactor(String value) {
        this.value = new BigInteger(value);
    }

    @Override
    public String toString() {
        return value.toString();
    }

    @Override
    public boolean equals(Object another) {
        return another instanceof ConstFactor
                && this.value.compareTo(((ConstFactor) another).value) == 0;
    }

    @Override
    public boolean withSign() {
        return this.toString().charAt(0) == '-' || this.toString().charAt(0) == '+';
    }

    @Override
    public boolean isEmpty() {
        return this.value.compareTo(BigInteger.ONE) == 0;
    }

    @Override
    public ArrayList<FactorType> differential() {
        ArrayList<FactorType> ret = new ArrayList<>();
        ret.add(new ConstFactor("0"));
        return ret;
    }

    @Override
    public boolean canMerge(FactorType another) {
        return another instanceof ConstFactor;
    }

    @Override
    public void mulUpdate(FactorType another) {
        if (another instanceof ConstFactor) {
            this.value = this.value.multiply(((ConstFactor) another).value);
        }
    }

    public void addUpdate(ConstFactor another) {
        this.value = this.value.add(another.value);
    }

    public boolean isZero() {
        return this.value.compareTo(BigInteger.ZERO) == 0;
    }
}
