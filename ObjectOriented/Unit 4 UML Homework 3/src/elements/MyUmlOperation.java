package elements;

import com.oocourse.uml3.models.common.Direction;
import com.oocourse.uml3.models.common.ElementType;
import com.oocourse.uml3.models.common.Visibility;
import com.oocourse.uml3.models.elements.UmlElement;
import com.oocourse.uml3.models.elements.UmlOperation;
import com.oocourse.uml3.models.elements.UmlParameter;

import java.util.ArrayList;

public class MyUmlOperation extends MyUmlElement {

    private final UmlElement umlOperation;
    private final ArrayList<UmlParameter> parameters;
    private int typeCode;
    /*private boolean hasParam;
    private boolean hasReturn;*/

    public MyUmlOperation(UmlElement umlOperation) {
        this.umlOperation = umlOperation;
        this.parameters = new ArrayList<>();
        this.typeCode = 0;
        /*this.hasParam = false;
        this.hasReturn = false;*/
    }

    @Override
    public int hashCode() {
        return umlOperation.hashCode();
    }

    @Override
    public boolean equals(Object o) {
        if (o == this) {
            return true;
        } else if (o instanceof MyUmlOperation) {
            MyUmlOperation that = (MyUmlOperation) o;
            return this.umlOperation.equals(that.umlOperation);
        } else {
            return false;
        }
    }

    @Override
    public UmlElement getUmlElement() {
        return this.umlOperation;
    }

    @Override
    public ElementType getElementType() {
        return ElementType.UML_OPERATION;
    }

    @Override
    public String getId() {
        return umlOperation.getId();
    }

    @Override
    public String getName() {
        return umlOperation.getName();
    }

    @Override
    public String getParentId() {
        return umlOperation.getParentId();
    }

    public Visibility getVisibility() {
        return ((UmlOperation) umlOperation).getVisibility();
    }

    public void addParam(UmlParameter parameter) {
        this.parameters.add(parameter);
        Direction direction = parameter.getDirection();
        if (direction == Direction.IN || direction == Direction.OUT
                || direction == Direction.INOUT) {
            //this.hasParam = true;
            this.typeCode = this.typeCode | 2;
        } else if (direction == Direction.RETURN) {
            //this.hasReturn = true;
            this.typeCode = this.typeCode | 1;
        }
    }

    public int typeCode() {
        return this.typeCode;
    }
}
