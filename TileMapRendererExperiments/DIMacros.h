#pragma once

#define DI_Decl(className, ... ) \
	className##( __VA_ARGS__ );\
	static std::unique_ptr<##className##> className##Factory(  __VA_ARGS__  );

#define DI_FactoryImpl(className, ...) return std::make_unique<##className##>( __VA_ARGS__ );
#define DI_FactoryDecl(className) std::unique_ptr<##className##> className##::className##Factory

/*

// example usage of di library 

class Class3
{
public:
    DI_Decl(Class3) // creates declaration for ctor and factory function
    std::string msg = "world";
};

Class3::Class3(){}
DI_FactoryDecl(Class3)()
{
    DI_FactoryImpl(Class3)
}

class Class2
{
public:
    DI_Decl(Class2)
    std::string msg = "hello";
};

Class2::Class2() {}
DI_FactoryDecl(Class2)()
{
    DI_FactoryImpl(Class2)
}

class Class1
{
public:

DI_Decl(Class1, Class2*, Class3*)

private:
    Class2* m_class2;
    Class3* m_class3;
};

Class1::Class1(Class2* class2, Class3* class3)
    :m_class2(class2),
    m_class3(class3)
{
}
DI_FactoryDecl(Class1)(Class2* class2, Class3* class3)
{
    DI_FactoryImpl(Class1, class2, class3);
}

void test()
{
    di_config di;
    di.add(&Class1::Class1Factory);
    di.add(&Class2::Class2Factory);
    di.add(&Class3::Class3Factory);

    injector i = di.build_injector();
    Class1* svc = i.get_instance<Class1>();
}


*/