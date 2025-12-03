class Context {

};

// 稳定点：抽象去解决它
// 变化点：扩展（继承和组合）去解决它
class ProStategy {
public:
    virtual double CalcPro(const Context &ctx) = 0;
    virtual ~ProStategy(); 
};
// cpp
class VAC_Spring : public ProStategy {
public:
    virtual double CalcPro(const Context &ctx){
    }
};

class VAC_Spring_v2 : public VAC_Spring {
public:
    virtual double CalcPro(const Context &ctx){
        //....
    }
};

class VAC_worker : public ProStategy {
public:
    virtual double CalcPro(const Context &ctx){}
};

// cpp
class VAC_QiXi : public ProStategy {
public:
    virtual double CalcPro(const Context &ctx){}
};
class VAC_QiXi1  : public VAC_QiXi {
public:
    virtual double CalcPro(const Context &ctx){}
};
// cpp
class VAC_Wuyi : public ProStategy {
public:
    virtual double CalcPro(const Context &ctx){}
};
// cpp
class VAC_GuoQing : public ProStategy {
public:
    virtual double CalcPro(const Context &ctx){}
};
class VAC_GuoQing2 : public VAC_GuoQing {
public:
    virtual double CalcPro(const Context &ctx){}
};

class VAC_Shengdan : public ProStategy {
public:
    virtual double CalcPro(const Context &ctx){}
};

// 设计原则：接口隔离原则
// 组合、继承
// 组合基类指针
// 两种方法：1. 采用具体接口选择算法  2. 依赖注入
class Promotion {
public:
    Promotion(ProStategy *sss = nullptr) : s(sss){}
    ~Promotion(){}
    void Choose(ProStategy *sss) {
        // 条件选择
        if (sss != nullptr) {
            s = sss;
        }
    }
    double CalcPromotion(const Context &ctx){
        if (s != nullptr) {
            return s->CalcPro(ctx);
        }
        return 0.0L;
    }
private:
    ProStategy *s;
};

int main () {
    Context ctx;
    ProStategy *s = new VAC_QiXi1();
    Promotion *p = new Promotion(s);
    p->Choose(new VAC_GuoQing2());
    p->CalcPromotion(ctx);
    return 0;
}