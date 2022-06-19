package elements;

import com.oocourse.uml1.models.common.ElementType;
import com.oocourse.uml1.models.elements.UmlElement;

public abstract class MyUmlElement {

    @Override
    public abstract int hashCode();

    @Override
    public abstract boolean equals(Object o);

    public abstract UmlElement getUmlElement();

    public abstract ElementType getElementType();

    public abstract String getId();

    public abstract String getName();

    public abstract String getParentId();
}
