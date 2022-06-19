package elements;

import com.oocourse.uml3.interact.common.AttributeClassInformation;
import com.oocourse.uml3.interact.common.AttributeQueryType;
import com.oocourse.uml3.interact.exceptions.user.AttributeDuplicatedException;
import com.oocourse.uml3.interact.exceptions.user.AttributeNotFoundException;
import com.oocourse.uml3.models.common.ElementType;
import com.oocourse.uml3.models.common.Visibility;
import com.oocourse.uml3.models.elements.UmlAssociationEnd;
import com.oocourse.uml3.models.elements.UmlAttribute;
import com.oocourse.uml3.models.elements.UmlElement;
import common.MyOperationQueryType;
import interfaces.Generalizable;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Queue;

public class MyUmlClass extends MyUmlClassOrInterface implements Generalizable {

    private final UmlElement umlClass;
    private MyUmlClass parent;
    private final ArrayList<UmlAttribute> attributes;
    private final ArrayList<MyUmlOperation> operations;
    private final ArrayList<MyUmlClass> associations;
    private final ArrayList<UmlAssociationEnd> associationEnds;
    private final ArrayList<MyUmlInterface> interfaces;
    private int associationCount;

    public MyUmlClass(UmlElement umlClass) {
        this.umlClass = umlClass;
        this.parent = null;
        this.attributes = new ArrayList<>();
        this.operations = new ArrayList<>();
        this.associations = new ArrayList<>();
        this.associationEnds = new ArrayList<>();
        this.interfaces = new ArrayList<>();
        this.associationCount = 0;
    }

    @Override
    public int hashCode() {
        return umlClass.hashCode();
    }

    @Override
    public boolean equals(Object o) {
        if (o == this) {
            return true;
        } else if (o instanceof MyUmlClass) {
            MyUmlClass that = (MyUmlClass) o;
            return this.umlClass.equals(that.umlClass);
        } else {
            return false;
        }
    }

    @Override
    public UmlElement getUmlElement() {
        return this.umlClass;
    }

    @Override
    public ElementType getElementType() {
        return ElementType.UML_CLASS;
    }

    @Override
    public String getId() {
        return umlClass.getId();
    }

    @Override
    public String getName() {
        return umlClass.getName();
    }

    @Override
    public String getParentId() {
        return umlClass.getParentId();
    }

    @Override
    public void addParent(Generalizable parent) {
        this.parent = (MyUmlClass) parent;
    }

    public void addOperation(MyUmlOperation operation) {
        this.operations.add(operation);
    }

    public void addAttribute(UmlAttribute attribute) {
        this.attributes.add(attribute);
    }

    public void addAssociation(MyUmlElement element) {
        associationCount++;
        if (element.getElementType() == ElementType.UML_CLASS) {
            this.associations.add((MyUmlClass) element);
        }
    }

    public void addAssociationEnd(UmlAssociationEnd end) {
        this.associationEnds.add(end);
    }

    public void implementInterface(MyUmlInterface myUmlInterface) {
        this.interfaces.add(myUmlInterface);
    }

    public int getClassOperationCount(MyOperationQueryType type) {
        int count = 0;
        if (type == MyOperationQueryType.ALL) {
            return operations.size();
        } else if (type == MyOperationQueryType.PARAM_RET) {
            for (MyUmlOperation operation : operations) {
                if (operation.typeCode() == 3) {
                    count++;
                }
            }
        } else if (type == MyOperationQueryType.PARAM_NO_RET) {
            for (MyUmlOperation operation : operations) {
                if (operation.typeCode() == 2) {
                    count++;
                }
            }
        } else if (type == MyOperationQueryType.NO_PARAM_RET) {
            for (MyUmlOperation operation : operations) {
                if (operation.typeCode() == 1) {
                    count++;
                }
            }
        } else if (type == MyOperationQueryType.NO_PARAM_NO_RET) {
            for (MyUmlOperation operation : operations) {
                if (operation.typeCode() == 0) {
                    count++;
                }
            }
        } else if (type == MyOperationQueryType.BOTH_RET) {
            for (MyUmlOperation operation : operations) {
                if ((operation.typeCode() & 1) == 1) {
                    count++;
                }
            }
        } else if (type == MyOperationQueryType.BOTH_NO_RET) {
            for (MyUmlOperation operation : operations) {
                if ((operation.typeCode() & 1) == 0) {
                    count++;
                }
            }
        } else if (type == MyOperationQueryType.PARAM_BOTH) {
            for (MyUmlOperation operation : operations) {
                if ((operation.typeCode() & 2) == 2) {
                    count++;
                }
            }
        } else if (type == MyOperationQueryType.NO_PARAM_BOTH) {
            for (MyUmlOperation operation : operations) {
                if ((operation.typeCode() & 2) == 0) {
                    count++;
                }
            }
        }
        return count;
    }

    public int getClassAttributeCount(AttributeQueryType type) {
        if (type == AttributeQueryType.ALL) {
            MyUmlClass myUmlClass = this;
            int count = 0;
            while (myUmlClass != null) {
                count += myUmlClass.attributes.size();
                myUmlClass = myUmlClass.parent;
            }
            return count;
        } else {
            return this.attributes.size();
        }
    }

    public int getClassAssociationCount() {
        int count = 0;
        MyUmlClass myUmlClass = this;
        while (myUmlClass != null) {
            count += myUmlClass.associationCount;
            myUmlClass = myUmlClass.parent;
        }
        return count;
    }

    public List<String> getClassAssociatedClassList() {
        ArrayList<String> names = new ArrayList<>();
        HashSet<MyUmlClass> flag = new HashSet<>();
        MyUmlClass myUmlClass = this;
        while (myUmlClass != null) {
            for (MyUmlClass umlClass : myUmlClass.associations) {
                if (!flag.contains(umlClass)) {
                    names.add(umlClass.getName());
                    flag.add(umlClass);
                }
            }
            myUmlClass = myUmlClass.parent;
        }
        return names;
    }

    public Map<Visibility, Integer> getClassOperationVisibility(String operationName) {
        int publicCount = 0;
        int protectedCount = 0;
        int privateCount = 0;
        int packageCount = 0;
        for (MyUmlOperation operation : operations) {
            if (operation.getName().equals(operationName)) {
                switch (operation.getVisibility()) {
                    case PUBLIC:
                        publicCount++;
                        break;
                    case PROTECTED:
                        protectedCount++;
                        break;
                    case PRIVATE:
                        privateCount++;
                        break;
                    case PACKAGE:
                        packageCount++;
                        break;
                    default:
                        break;
                }
            }
        }
        HashMap<Visibility, Integer> ret = new HashMap<>();
        ret.put(Visibility.PUBLIC, publicCount);
        ret.put(Visibility.PROTECTED, protectedCount);
        ret.put(Visibility.PRIVATE, privateCount);
        ret.put(Visibility.PACKAGE, packageCount);
        return ret;
    }

    public Visibility getClassAttributeVisibility(String attributeName)
            throws AttributeNotFoundException, AttributeDuplicatedException {
        UmlAttribute attribute = null;
        MyUmlClass myUmlClass = this;
        while (myUmlClass != null) {
            for (UmlAttribute umlAttribute : myUmlClass.attributes) {
                if (umlAttribute.getName().equals(attributeName)) {
                    if (attribute != null) {
                        throw new AttributeDuplicatedException(this.getName(), attributeName);
                    } else {
                        attribute = umlAttribute;
                    }
                }
            }
            myUmlClass = myUmlClass.parent;
        }
        if (attribute == null) {
            throw new AttributeNotFoundException(this.getName(), attributeName);
        } else {
            return attribute.getVisibility();
        }
    }

    public String getTopParentClass() {
        MyUmlClass myUmlClass = this;
        while (myUmlClass.parent != null) {
            myUmlClass = myUmlClass.parent;
        }
        return myUmlClass.getName();
    }

    public List<String> getImplementInterfaceList() {
        ArrayList<String> ret = new ArrayList<>();
        HashSet<MyUmlInterface> flag = new HashSet<>();
        Queue<MyUmlInterface> queue = new LinkedList<>();
        MyUmlClass myUmlClass = this;
        while (myUmlClass != null) {
            for (MyUmlInterface myUmlInterface : myUmlClass.interfaces) {
                if (!flag.contains(myUmlInterface)) {
                    queue.add(myUmlInterface);
                    flag.add(myUmlInterface);
                }
            }
            myUmlClass = myUmlClass.parent;
        }
        while (!queue.isEmpty()) {
            MyUmlInterface myUmlInterface = queue.poll();
            ret.add(myUmlInterface.getName());
            for (MyUmlInterface umlInterface : myUmlInterface.getParents()) {
                if (!flag.contains(umlInterface)) {
                    queue.add(umlInterface);
                    flag.add(umlInterface);
                }
            }
        }
        return ret;
    }

    public List<AttributeClassInformation> getInformationNotHidden() {
        ArrayList<AttributeClassInformation> ret = new ArrayList<>();
        MyUmlClass myUmlClass = this;
        while (myUmlClass != null) {
            for (UmlAttribute attribute : myUmlClass.attributes) {
                if (attribute.getVisibility() != Visibility.PRIVATE) {
                    ret.add(new AttributeClassInformation(
                            attribute.getName(), myUmlClass.getName()
                    ));
                }
            }
            myUmlClass = myUmlClass.parent;
        }
        return ret;
    }

    public List<String> getNameList() {
        ArrayList<String> list = new ArrayList<>();
        for (UmlAttribute attribute : attributes) {
            list.add(attribute.getName());
        }
        for (UmlAssociationEnd end : associationEnds) {
            list.add(end.getName());
        }
        return list;
    }

    public MyUmlClass getParentClass() {
        return parent;
    }

    @Override
    public ArrayList<MyUmlClassOrInterface> getParentsClassOrInterface() {
        ArrayList<MyUmlClassOrInterface> ret = new ArrayList<>(interfaces);
        if (parent != null) {
            ret.add(parent);
        }
        return ret;
    }
}
