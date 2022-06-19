package structures.factors;

import interfaces.FactorType;

import java.math.BigInteger;
import java.util.ArrayList;

public class CosFactor implements FactorType {

    private FactorType inner;
    private BigInteger index;

    public CosFactor(FactorType inner, BigInteger index) {
        this.inner = inner;
        this.index = index;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("cos(");
        sb.append(inner.toString());
        sb.append(")");
        if (index.compareTo(BigInteger.ONE) != 0) {
            sb.append("**");
            sb.append(index.toString());
        }
        if (sb.toString().isEmpty()) {
            return "1";
        }
        return sb.toString();
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
        return another instanceof CosFactor
                && this.inner.equals(((CosFactor) another).inner);
    }

    @Override
    public void mulUpdate(FactorType another) {
        if (another instanceof CosFactor) {
            this.index = this.index.add(((CosFactor) another).index);
        }
    }

    @Override
    public boolean equals(Object another) {
        return another instanceof CosFactor
                && this.index.compareTo(((CosFactor) another).index) == 0
                && this.inner.equals(((CosFactor) another).inner);
    }

    @Override
    public ArrayList<FactorType> differential() {
        ArrayList<FactorType> ret = new ArrayList<>();
        if (this.index.compareTo(BigInteger.ONE) == 0) {
            ret.add(new ConstFactor(BigInteger.ONE.negate()));
            ret.add(new SinFactor(this.inner, this.index));
        } else {
            ret.add(new ConstFactor(this.index.negate()));
            ret.add(new CosFactor(this.inner, this.index.subtract(BigInteger.ONE)));
            ret.add(new SinFactor(this.inner, BigInteger.ONE));
        }
        ret.addAll(this.inner.differential());
        return ret;
    }
}
