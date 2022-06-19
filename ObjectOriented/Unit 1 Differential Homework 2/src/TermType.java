import java.math.BigInteger;
import java.util.ArrayList;

public class TermType implements Comparable<TermType>, Cloneable {

    private BigInteger coefficient; // k
    private BigInteger varIndex; // a
    private BigInteger sinIndex; // b
    private BigInteger cosIndex; // c

    private final BigInteger zero = new BigInteger("0");
    private final BigInteger one = new BigInteger("1");
    private final BigInteger two = new BigInteger("2");

    TermType(BigInteger coefficient, BigInteger varIndex,
             BigInteger sinIndex, BigInteger cosIndex) {
        this.coefficient = coefficient;
        this.varIndex = varIndex;
        this.sinIndex = sinIndex;
        this.cosIndex = cosIndex;
    }

    @Override
    public String toString() {
        if (this.isEmpty()) {
            return "0";
        }
        if (this.isConstance()) {
            return coefficient.toString();
        }
        StringBuilder stringBuilder = new StringBuilder();
        if (coefficient.compareTo(one.negate()) == 0) {
            stringBuilder.append("-");
        } else if (coefficient.compareTo(one) != 0) {
            stringBuilder.append(coefficient.toString()).append("*");
        }
        boolean addDirectly = true;
        if (varIndex.compareTo(zero) != 0) {
            stringBuilder.append("x");
            if (varIndex.compareTo(two) == 0) {
                stringBuilder.append("*x");
            } else if (varIndex.compareTo(one) != 0) {
                stringBuilder.append("**").append(varIndex.toString());
            }
            addDirectly = false;
        }
        if (sinIndex.compareTo(zero) != 0) {
            if (!addDirectly) {
                stringBuilder.append("*");
            }
            stringBuilder.append("sin(x)");
            if (sinIndex.compareTo(one) != 0) {
                stringBuilder.append("**").append(sinIndex.toString());
            }
            addDirectly = false;
        }
        if (cosIndex.compareTo(zero) != 0) {
            if (!addDirectly) {
                stringBuilder.append("*");
            }
            stringBuilder.append("cos(x)");
            if (cosIndex.compareTo(one) != 0) {
                stringBuilder.append("**").append(cosIndex.toString());
            }
        }
        return stringBuilder.toString();
    }

    @Override
    public Object clone() {
        return new TermType(
                new BigInteger(this.coefficient.toString()),
                new BigInteger(this.varIndex.toString()),
                new BigInteger(this.sinIndex.toString()),
                new BigInteger(this.cosIndex.toString())
        );
    }

    @Override
    public int compareTo(TermType anotherTerm) {
        if (this.varIndex.compareTo(anotherTerm.varIndex) > 0) {
            return 1;
        } else if (this.varIndex.compareTo(anotherTerm.varIndex) == 0) {
            if (this.sinIndex.compareTo(anotherTerm.sinIndex) > 0) {
                return 1;
            } else if (this.sinIndex.compareTo(anotherTerm.sinIndex) == 0) {
                if (this.cosIndex.compareTo(anotherTerm.cosIndex) > 0) {
                    return 1;
                } else if (this.cosIndex.compareTo(anotherTerm.cosIndex) == 0) {
                    return 0;
                } else {
                    return -1;
                }
            } else {
                return -1;
            }
        } else {
            return -1;
        }
    }

    public boolean isEmpty() {
        return coefficient.compareTo(zero) == 0;
    }

    public boolean isPositive() {
        return coefficient.compareTo(zero) > 0;
    }

    public boolean isConstance() {
        return this.varIndex.compareTo(zero) == 0
                && this.sinIndex.compareTo(zero) == 0
                && this.cosIndex.compareTo(zero) == 0;
    }

    public void setEmpty() {
        this.coefficient = zero;
    }

    public TermType add(TermType anotherTerm) {
        if (this.compareTo(anotherTerm) != 0) {
            return null;
        }
        return new TermType(this.coefficient.add(anotherTerm.coefficient),
                this.varIndex, this.sinIndex, this.cosIndex);
    }

    public ArrayList<TermType> differential() {
        ArrayList<TermType> returnArray = new ArrayList<>();
        if (varIndex.compareTo(BigInteger.ZERO) == 0) {
            if (sinIndex.compareTo(BigInteger.ZERO) == 0) {
                if (cosIndex.compareTo(BigInteger.ZERO) == 0) { // k
                    returnArray.add(new TermType(zero, zero, zero, zero));
                } else { // k * cos(x)^c
                    returnArray.add(new TermType(coefficient.multiply(cosIndex).negate(),
                            zero, one, cosIndex.subtract(one)));
                }
            } else {
                if (cosIndex.compareTo(BigInteger.ZERO) == 0) { // k * sin(x)^b
                    returnArray.add(new TermType(coefficient.multiply(sinIndex), zero,
                            sinIndex.subtract(one), one));
                } else { // k * sin(x)^b * cos(x)^c
                    returnArray.add(new TermType(coefficient.multiply(sinIndex), zero,
                            sinIndex.subtract(one), cosIndex.add(one)));
                    returnArray.add(new TermType(coefficient.multiply(cosIndex).negate(),
                            zero, sinIndex.add(one), cosIndex.subtract(one)));
                }
            }
        } else {
            if (sinIndex.compareTo(BigInteger.ZERO) == 0) {
                if (cosIndex.compareTo(BigInteger.ZERO) == 0) { // k * x^a
                    returnArray.add(new TermType(coefficient.multiply(varIndex),
                            varIndex.subtract(one), zero, zero));
                } else { // k * x^a * cos(x)^c
                    returnArray.add(new TermType(coefficient.multiply(varIndex),
                            varIndex.subtract(one), zero, cosIndex));
                    returnArray.add(new TermType(coefficient.multiply(cosIndex).negate(),
                            varIndex, one, cosIndex.subtract(one)));
                }
            } else {
                if (cosIndex.compareTo(BigInteger.ZERO) == 0) { // k * x^a * sin(x)^b
                    returnArray.add(new TermType(coefficient.multiply(varIndex),
                            varIndex.subtract(one), sinIndex, zero));
                    returnArray.add(new TermType(coefficient.multiply(sinIndex),
                            varIndex, sinIndex.subtract(one), one));
                } else { // k * x^a * sin(x)^b * cos(x)^c
                    returnArray.add(new TermType(coefficient.multiply(varIndex),
                            varIndex.subtract(one), sinIndex, cosIndex));
                    returnArray.add(new TermType(coefficient.multiply(sinIndex),
                            varIndex, sinIndex.subtract(one), cosIndex.add(one)));
                    returnArray.add(new TermType(coefficient.multiply(cosIndex).negate(),
                            varIndex, sinIndex.add(one), cosIndex.subtract(one)));
                }
            }
        }
        return returnArray;
    }

    // if no need or unable to merge return null
    public ArrayList<TermType> merge(TermType anotherTerm) {
        if (this == anotherTerm) {
            return null;
        }
        ArrayList<TermType> returnArr = new ArrayList<>();
        if (this.compareTo(anotherTerm) == 0) {
            returnArr.add(new TermType(this.coefficient.add(anotherTerm.coefficient),
                    this.varIndex, this.sinIndex, this.cosIndex));
        } else if (this.varIndex.compareTo(anotherTerm.varIndex) == 0
                && this.sinIndex.subtract(two).compareTo(anotherTerm.sinIndex) == 0
                && anotherTerm.cosIndex.subtract(two).compareTo(this.cosIndex) == 0) {
            if (this.coefficient.compareTo(anotherTerm.coefficient) == 0) {
                returnArr.add(new TermType(this.coefficient, this.varIndex,
                        anotherTerm.sinIndex, this.cosIndex));
            } else if (this.coefficient.multiply(anotherTerm.coefficient).compareTo(zero) < 0) {
                returnArr.add(new TermType(this.coefficient.subtract(anotherTerm.coefficient),
                        this.varIndex, this.sinIndex, this.cosIndex));
                returnArr.add(new TermType(anotherTerm.coefficient, anotherTerm.varIndex,
                        anotherTerm.sinIndex, this.cosIndex));
            } else if (this.coefficient.abs().compareTo(anotherTerm.coefficient.abs()) > 0) {
                returnArr.add(new TermType(this.coefficient.subtract(anotherTerm.coefficient),
                        this.varIndex, this.sinIndex, this.cosIndex));
                returnArr.add(new TermType(anotherTerm.coefficient, anotherTerm.varIndex,
                        anotherTerm.sinIndex, this.cosIndex));
            } else {
                returnArr.add(new TermType(anotherTerm.coefficient.subtract(this.coefficient),
                        anotherTerm.varIndex, anotherTerm.sinIndex, anotherTerm.cosIndex));
                returnArr.add(new TermType(this.coefficient, this.varIndex,
                        anotherTerm.sinIndex, this.cosIndex));
            }
        } else if (this.varIndex.compareTo(anotherTerm.varIndex) == 0
                && anotherTerm.sinIndex.subtract(two).compareTo(this.sinIndex) == 0
                && this.cosIndex.subtract(two).compareTo(anotherTerm.cosIndex) == 0) {
            returnArr = anotherTerm.merge(this);
            if (returnArr != null) {
                if (returnArr.size() == 2) {
                    TermType tempTerm = returnArr.get(0);
                    returnArr.set(0, returnArr.get(1));
                    returnArr.set(1, tempTerm);
                }
            }
        } else {
            return null;
        }
        return returnArr;
    }

    // switch from sin() to cos() or opposite
    public ArrayList<TermType> convert(TermType anotherTerm) {
        if (this == anotherTerm) {
            return null;
        }
        ArrayList<TermType> returnArr = new ArrayList<>();
        if (this.compareTo(anotherTerm) == 0) {
            returnArr.add(new TermType(this.coefficient.add(anotherTerm.coefficient),
                    this.varIndex, this.sinIndex, this.cosIndex));
        } else if (this.varIndex.compareTo(anotherTerm.varIndex) == 0
                && this.sinIndex.subtract(two).compareTo(anotherTerm.sinIndex) == 0
                && this.cosIndex.compareTo(anotherTerm.cosIndex) == 0) {
            if (this.coefficient.add(anotherTerm.coefficient).compareTo(zero) == 0) {
                returnArr.add(new TermType(anotherTerm.coefficient, anotherTerm.varIndex,
                        anotherTerm.sinIndex, anotherTerm.cosIndex.add(two)));
            } else {
                returnArr.add(new TermType(this.coefficient.add(anotherTerm.coefficient),
                        anotherTerm.varIndex, anotherTerm.sinIndex, anotherTerm.cosIndex));
                returnArr.add(new TermType(this.coefficient.negate(), this.varIndex,
                        anotherTerm.sinIndex, anotherTerm.cosIndex.add(two)));
            }
        } else if (this.varIndex.compareTo(anotherTerm.varIndex) == 0
                && this.sinIndex.compareTo(anotherTerm.sinIndex) == 0
                && this.cosIndex.subtract(two).compareTo(anotherTerm.cosIndex) == 0) {
            if (this.coefficient.add(anotherTerm.coefficient).compareTo(zero) == 0) {
                returnArr.add(new TermType(anotherTerm.coefficient, anotherTerm.varIndex,
                        anotherTerm.sinIndex.add(two), anotherTerm.cosIndex));
            } else {
                returnArr.add(new TermType(this.coefficient.add(anotherTerm.coefficient),
                        anotherTerm.varIndex, anotherTerm.sinIndex, anotherTerm.cosIndex));
                returnArr.add(new TermType(this.coefficient.negate(), this.varIndex,
                        anotherTerm.sinIndex.add(two), anotherTerm.cosIndex));
            }
        } else if (this.varIndex.compareTo(anotherTerm.varIndex) == 0
                && anotherTerm.sinIndex.subtract(two).compareTo(this.sinIndex) == 0
                && this.cosIndex.compareTo(anotherTerm.cosIndex) == 0) {
            returnArr = anotherTerm.convert(this);
            if (this.coefficient.add(anotherTerm.coefficient).compareTo(zero) == 0) {
                returnArr.add(0, new TermType(zero, zero, zero, zero));
            } else {
                TermType tempTerm = returnArr.get(0);
                returnArr.set(0, returnArr.get(1));
                returnArr.set(1, tempTerm);
            }
        } else if (this.varIndex.compareTo(anotherTerm.varIndex) == 0
                && this.sinIndex.compareTo(anotherTerm.sinIndex) == 0
                && anotherTerm.cosIndex.subtract(two).compareTo(this.cosIndex) == 0) {
            returnArr = anotherTerm.convert(this);
            if (this.coefficient.add(anotherTerm.coefficient).compareTo(zero) == 0) {
                returnArr.add(0, new TermType(zero, zero, zero, zero));
            } else {
                TermType tempTerm = returnArr.get(0);
                returnArr.set(0, returnArr.get(1));
                returnArr.set(1, tempTerm);
            }
        } else {
            return null;
        }
        return returnArr;
    }
}
