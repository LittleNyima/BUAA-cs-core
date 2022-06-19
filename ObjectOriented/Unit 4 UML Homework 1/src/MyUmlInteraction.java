import com.oocourse.uml1.interact.common.AttributeClassInformation;
import com.oocourse.uml1.interact.common.AttributeQueryType;
import com.oocourse.uml1.interact.common.OperationQueryType;
import com.oocourse.uml1.interact.exceptions.user.AttributeDuplicatedException;
import com.oocourse.uml1.interact.exceptions.user.AttributeNotFoundException;
import com.oocourse.uml1.interact.exceptions.user.ClassDuplicatedException;
import com.oocourse.uml1.interact.exceptions.user.ClassNotFoundException;
import com.oocourse.uml1.interact.exceptions.user.ConflictQueryTypeException;
import com.oocourse.uml1.interact.format.UmlInteraction;
import com.oocourse.uml1.models.common.ElementType;
import com.oocourse.uml1.models.common.Visibility;
import com.oocourse.uml1.models.elements.UmlAssociation;
import com.oocourse.uml1.models.elements.UmlAssociationEnd;
import com.oocourse.uml1.models.elements.UmlAttribute;
import com.oocourse.uml1.models.elements.UmlElement;
import com.oocourse.uml1.models.elements.UmlGeneralization;
import com.oocourse.uml1.models.elements.UmlInterfaceRealization;
import com.oocourse.uml1.models.elements.UmlParameter;
import common.MyOperationQueryType;
import elements.MyUmlClass;
import elements.MyUmlElement;
import elements.MyUmlInterface;
import elements.MyUmlOperation;
import elements.MyUmlOthers;
import interfaces.Generalizable;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class MyUmlInteraction implements UmlInteraction {

    private final HashMap<String, MyUmlElement> idElementMap;
    private final HashMap<String, ArrayList<MyUmlClass>> nameClassMap;
    private int classCount;

    public MyUmlInteraction(UmlElement... elements) {
        this.idElementMap = new HashMap<>();
        this.nameClassMap = new HashMap<>();
        this.classCount = 0;
        this.initObjects(elements);
        this.initAttributes(elements);
    }

    private void initObjects(UmlElement... elements) {
        for (UmlElement element : elements) {
            ElementType type = element.getElementType();
            if (type == ElementType.UML_CLASS) {
                MyUmlClass myUmlClass = new MyUmlClass(element);
                idElementMap.put(element.getId(), myUmlClass);
                classCount++;
                if (!nameClassMap.containsKey(element.getName())) {
                    ArrayList<MyUmlClass> list = new ArrayList<>();
                    list.add(myUmlClass);
                    nameClassMap.put(element.getName(), list);
                } else {
                    nameClassMap.get(element.getName()).add(myUmlClass);
                }
            } else if (type == ElementType.UML_INTERFACE) {
                idElementMap.put(element.getId(), new MyUmlInterface(element));
            } else if (type == ElementType.UML_OPERATION) {
                idElementMap.put(element.getId(), new MyUmlOperation(element));
            } else {
                idElementMap.put(element.getId(), new MyUmlOthers(element));
            }
        }
    }

    private void initAttributes(UmlElement... elements) {
        ArrayList<UmlElement> paramList = new ArrayList<>();
        for (UmlElement element : elements) {
            ElementType type = element.getElementType();
            if (type == ElementType.UML_ASSOCIATION) {
                UmlAssociationEnd end1 = (UmlAssociationEnd) idElementMap
                        .get(((UmlAssociation) element).getEnd1()).getUmlElement();
                UmlAssociationEnd end2 = (UmlAssociationEnd) idElementMap
                        .get(((UmlAssociation) element).getEnd2()).getUmlElement();
                MyUmlElement myUmlElement1 = idElementMap.get(end1.getReference());
                MyUmlElement myUmlElement2 = idElementMap.get(end2.getReference());
                if (myUmlElement1.getElementType() == ElementType.UML_CLASS) {
                    ((MyUmlClass) myUmlElement1).addAssociation(myUmlElement2);
                }
                if (myUmlElement2.getElementType() == ElementType.UML_CLASS) {
                    ((MyUmlClass) myUmlElement2).addAssociation(myUmlElement1);
                }
            } else if (type == ElementType.UML_ATTRIBUTE) {
                ((MyUmlClass) idElementMap.get(element.getParentId()))
                        .addAttribute((UmlAttribute) element);
            } else if (type == ElementType.UML_GENERALIZATION) {
                ((Generalizable) idElementMap.get(((UmlGeneralization) element).getSource()))
                        .addParent((Generalizable) idElementMap
                                .get(((UmlGeneralization) element).getTarget()));
            } else if (type == ElementType.UML_INTERFACE_REALIZATION) {
                ((MyUmlClass) idElementMap.get(((UmlInterfaceRealization) element).getSource()))
                        .implementInterface((MyUmlInterface) idElementMap
                                .get(((UmlInterfaceRealization) element).getTarget()));
            } else if (type == ElementType.UML_OPERATION) {
                MyUmlElement myUmlElement = idElementMap.get(element.getParentId());
                if (myUmlElement.getElementType() == ElementType.UML_CLASS) {
                    ((MyUmlClass) myUmlElement)
                            .addOperation((MyUmlOperation) idElementMap.get(element.getId()));
                }
            } else if (type == ElementType.UML_PARAMETER) {
                paramList.add(element);
            }
        }
        for (UmlElement element : paramList) {
            ((MyUmlOperation) idElementMap.get(element.getParentId()))
                    .addParam((UmlParameter) element);
        }
    }

    private MyUmlClass findClassByName(String className)
            throws ClassNotFoundException, ClassDuplicatedException {
        if (!nameClassMap.containsKey(className)) {
            throw new ClassNotFoundException(className);
        } else {
            ArrayList<MyUmlClass> list = nameClassMap.get(className);
            if (list.size() != 1) {
                throw new ClassDuplicatedException(className);
            } else {
                return list.get(0);
            }
        }
    }

    @Override
    public int getClassCount() {
        return this.classCount;
    }

    @Override
    public int getClassOperationCount(String className, OperationQueryType[] queryTypes)
            throws ClassNotFoundException, ClassDuplicatedException, ConflictQueryTypeException {
        MyUmlClass myUmlClass = findClassByName(className);
        MyOperationQueryType type = sortQueryTypes(queryTypes);
        return myUmlClass.getClassOperationCount(type);
    }

    private MyOperationQueryType sortQueryTypes(OperationQueryType[] queryTypes)
            throws ConflictQueryTypeException {
        Boolean hasParam = null;
        Boolean hasReturn = null;
        for (OperationQueryType query : queryTypes) {
            switch (query) {
                case PARAM:
                    if (hasParam != null && !hasParam) {
                        throw new ConflictQueryTypeException();
                    } else {
                        hasParam = true;
                    }
                    break;
                case NON_PARAM:
                    if (hasParam != null && hasParam) {
                        throw new ConflictQueryTypeException();
                    } else {
                        hasParam = false;
                    }
                    break;
                case RETURN:
                    if (hasReturn != null && !hasReturn) {
                        throw new ConflictQueryTypeException();
                    } else {
                        hasReturn = true;
                    }
                    break;
                case NON_RETURN:
                    if (hasReturn != null && hasReturn) {
                        throw new ConflictQueryTypeException();
                    } else {
                        hasReturn = false;
                    }
                    break;
                default:
            }
        }
        if (hasParam == null && hasReturn == null) {
            return MyOperationQueryType.ALL;
        } else if (hasParam == null) {
            if (hasReturn) {
                return MyOperationQueryType.BOTH_RET;
            } else {
                return MyOperationQueryType.BOTH_NO_RET;
            }
        } else if (hasReturn == null) {
            if (hasParam) {
                return MyOperationQueryType.PARAM_BOTH;
            } else {
                return MyOperationQueryType.NO_PARAM_BOTH;
            }
        } else if (hasParam && hasReturn) {
            return MyOperationQueryType.PARAM_RET;
        } else if (hasParam) {
            return MyOperationQueryType.PARAM_NO_RET;
        } else if (hasReturn) {
            return MyOperationQueryType.NO_PARAM_RET;
        } else {
            return MyOperationQueryType.NO_PARAM_NO_RET;
        }
    }

    @Override
    public int getClassAttributeCount(String className, AttributeQueryType queryType)
            throws ClassNotFoundException, ClassDuplicatedException {
        MyUmlClass myUmlClass = findClassByName(className);
        return myUmlClass.getClassAttributeCount(queryType);
    }

    @Override
    public int getClassAssociationCount(String className)
            throws ClassNotFoundException, ClassDuplicatedException {
        MyUmlClass myUmlClass = findClassByName(className);
        return myUmlClass.getClassAssociationCount();
    }

    @Override
    public List<String> getClassAssociatedClassList(String className)
            throws ClassNotFoundException, ClassDuplicatedException {
        MyUmlClass myUmlClass = findClassByName(className);
        return myUmlClass.getClassAssociatedClassList();
    }

    @Override
    public Map<Visibility, Integer> getClassOperationVisibility(
            String className, String operationName
    ) throws ClassNotFoundException, ClassDuplicatedException {
        MyUmlClass myUmlClass = findClassByName(className);
        return myUmlClass.getClassOperationVisibility(operationName);
    }

    @Override
    public Visibility getClassAttributeVisibility(String className, String attributeName)
            throws ClassNotFoundException, ClassDuplicatedException,
            AttributeNotFoundException, AttributeDuplicatedException {
        MyUmlClass myUmlClass = findClassByName(className);
        return myUmlClass.getClassAttributeVisibility(attributeName);
    }

    @Override
    public String getTopParentClass(String className)
            throws ClassNotFoundException, ClassDuplicatedException {
        MyUmlClass myUmlClass = findClassByName(className);
        return myUmlClass.getTopParentClass();
    }

    @Override
    public List<String> getImplementInterfaceList(String className)
            throws ClassNotFoundException, ClassDuplicatedException {
        MyUmlClass myUmlClass = findClassByName(className);
        return myUmlClass.getImplementInterfaceList();
    }

    @Override
    public List<AttributeClassInformation> getInformationNotHidden(String className)
            throws ClassNotFoundException, ClassDuplicatedException {
        MyUmlClass myUmlClass = findClassByName(className);
        return myUmlClass.getInformationNotHidden();
    }
}
