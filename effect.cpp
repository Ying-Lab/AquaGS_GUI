#include "effect.h"

void display_effect(QString pr_csv_path,QTableView* original_tableview,QTableView* selected_tableview,QStringList effect_list)
{
    QStandardItemModel* original_model = new QStandardItemModel();
    QStandardItemModel* selected_model = new QStandardItemModel();
    original_model->setHorizontalHeaderLabels({"Phenotype", "P value"});
    selected_model->setHorizontalHeaderLabels({"Effect", "P value"});

    original_tableview->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    selected_tableview->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QFile csv_file(pr_csv_path);
    QStringList csv_list;
    csv_list.clear();
    if (!csv_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<endl << pr_csv_path << "is broken!"<< endl;
        return;
    }
    QTextStream stream(&csv_file);
    while (!stream.atEnd())
    {
        csv_list.push_back(stream.readLine());
    }
    csv_file.close();
    qDebug()<<endl<<"csv_list:"<<csv_list<<endl;

    unsigned count = 0;
    unsigned int i =0;
    unsigned int j =0;
    Q_FOREACH(QString str, csv_list)
    {
       QStringList valsplit = str.split(",");
       valsplit[0] = (valsplit[0].mid(1,valsplit[0].length()-2));
       valsplit[1] = (valsplit[1].mid(1,valsplit[1].length()-2));
       qDebug()<<"valsplit[0]"<<valsplit[0]<<"--"<<"valsplit[1]"<<valsplit[1];
       //qDebug()<<"count:"<<count;
       if((!(-1 == effect_list.indexOf(QString::number(count)))))
       {
           qDebug()<<endl<<"find the effect in the fixed_effect_list already,index is "<<effect_list.indexOf(QString::number(count))<<endl;
           selected_model->setItem(i, 0, new QStandardItem(valsplit[0]));//deleted ""
           if((!valsplit[1].count())||(valsplit[1] == "NA")||(valsplit[1] == "Err")||(valsplit[1] == "Fixed")||(valsplit[1] == "NA"))//NA or error happend,can't convert to double class
           {
               selected_model->setItem(i, 1, new QStandardItem("-"+valsplit[1]+"-"));
           }
           else {
               selected_model->setItem(i, 1, new QStandardItem(QString::number(valsplit[1].toDouble(),'f',6)));
           }
           //str->double->str
           i++;
       }
       else
       {
           original_model->setItem(j, 0, new QStandardItem(valsplit[0]));
           if((!valsplit[1].count())||valsplit[1] == "NA"||(valsplit[1] == "Err")||(valsplit[1] == "Fixed")||(valsplit[1] == "NaN"))
           {
               original_model->setItem(j, 1, new QStandardItem("-"+valsplit[1]+"-"));
           }
           else {
               original_model->setItem(j, 1, new QStandardItem(QString::number(valsplit[1].toDouble(),'f',6)));
           }
           j++;
       }
       count++;
    }
    original_tableview->setModel(original_model);
    selected_tableview->setModel(selected_model);
    original_tableview->show();
    selected_tableview->show();
}

void clean_effect_table(QTableView* tableview)
{
    QStandardItemModel* model = (QStandardItemModel*) tableview->model();
    model->clear();
    tableview->setModel(model);
    tableview->show();
}

bool fixed_effect_testing(QString input_path,QString output_path,unsigned int target_phenotype_index,QStringList fixed_effect_list)
{
    QString runPath = QDir::currentPath();
    runPath.append("/rscript/fixed_effect_testing.R");
    qDebug() << endl <<"runPath;" << runPath << endl;
    QString param;
    // The sequence of param is not changeable
    param.clear();
    param.append("Rscript");
    param.append(" ");
    param.append("/home/liang/Documents/AquaGS_GUI/rscript/fixed_effect_testing.R");
    param.append(" ");
    param.append(input_path);
    param.append(" ");
    param.append(output_path);
    param.append(" ");
    param.append(QString::number(target_phenotype_index));
    param.append(" ");
    param.append(QString::number(fixed_effect_list.length()));
    param.append(" ");
    for(int i = 0;i < fixed_effect_list.length();i++){
        param.append(fixed_effect_list[i]);
        param.append(" ");
    }
    qDebug()<< endl<<"display param :"<<param<< endl;
    QProcess display_process;
    display_process.execute(param);
    if(display_process.waitForStarted())
    {
        qDebug()<<"OUTLIER PROCESS STRATED";
        display_process.close();
        return false;
    }
    if(display_process.waitForFinished())
    {
        qDebug()<<"OUTLIER PROCESS FINISHED";
        display_process.close();
        return false;
    }
    display_process.close();
    return true;
}

bool random_effect_testing(QString input_path,QString output_path,unsigned int target_phenotype_index,QStringList fixed_effect_list)
{
    QString runPath = QDir::currentPath();
    runPath.append("/rscript/rondom_effect_testing.R");
    qDebug() << endl <<"runPath;" << runPath << endl;
    QString param;
    // The sequence of param is not changeable
    param.clear();
    param.append("Rscript");
    param.append(" ");
    param.append("/home/liang/Documents/AquaGS_GUI/rscript/random_effect_testing.R");
    param.append(" ");
    param.append(input_path);
    param.append(" ");
    param.append(output_path);
    param.append(" ");
    param.append(QString::number(target_phenotype_index));
    param.append(" ");
    param.append(QString::number(fixed_effect_list.length()));
    param.append(" ");
    for(int i = 0;i < fixed_effect_list.length();i++){
        param.append(fixed_effect_list[i]);
        param.append(" ");
    }

    qDebug()<< endl<<"display param :"<<param<< endl;
    QProcess display_process;
    display_process.execute(param);
    if(display_process.waitForStarted())
    {
        qDebug()<<"OUTLIER PROCESS STRATED";
        display_process.close();
        return false;
    }
    if(display_process.waitForFinished())
    {
        qDebug()<<"OUTLIER PROCESS FINISHED";
        display_process.close();
        return false;
    }
    display_process.close();
    return true;
}

bool prepare_effect(QString input_path,QString output_path,
                    QTableView* original_tableview,QTableView* selected_tableview,
                    unsigned int target_index,
                    unsigned  int random_flag,
                    QStringList fixed_effect_list,
                    QStringList random_feefct_list)
{
    QString effect_path = output_path;
    bool callbake = false;
    if(random_flag){
        effect_path.append("/random_effect.csv");
        qDebug() << endl << "random_effect_path:" << effect_path <<endl;
        callbake =  random_effect_testing(input_path,effect_path,target_index,fixed_effect_list);
        if(callbake)
        {
            display_effect(effect_path,original_tableview,selected_tableview,random_feefct_list);
        }
    }
    else{
        effect_path.append("/fixed_effect.csv");
        qDebug() << endl << "fixed_effect_path:" << effect_path <<endl;
        callbake =  fixed_effect_testing(input_path,effect_path,target_index,fixed_effect_list);
        if(callbake)
        {
            display_effect(effect_path,original_tableview,selected_tableview,fixed_effect_list);
        }
    }
    return callbake;

}

int get_phenotype_index(QTableView*tableview,QStringList phenotype_list)
{
    unsigned int selected_row = tableview->currentIndex().row();
    //qDebug()<<endl<<"selected row of  fixed is"<<selected_row;
    QStandardItemModel* model = (QStandardItemModel*) tableview->model();
    QModelIndex selected_index =model->index(selected_row,0);
    QString id = model->data(selected_index).toString();
    return phenotype_list.indexOf(id);
}

bool isTableView_empty(QTableView* tableview)
{
    QStandardItemModel* model = (QStandardItemModel*) tableview->model();
    if(model->item(0,0)==nullptr || model->item(0,0)->text().isEmpty())    return true;
    else    return false;
}

void add_item2effect_list(QTableView* original_tableview,QStringList phenotypelist,QStringList* fixed_effect_list)
{
    unsigned int selected_row = original_tableview->currentIndex().row();
    int select_phenotype_index =  get_phenotype_index(original_tableview,phenotypelist);

    bool Norepetition_flag = true;  //check the item is selected already or not
    unsigned int count_fixed_effect_list = (*fixed_effect_list).length();

    qDebug()<<endl<<"selected row of  effect is"<<selected_row;
    qDebug()<<endl<<"selected index of phenotypelist is "<<select_phenotype_index;
    qDebug()<<endl<<"the num of effect_list:"<<count_fixed_effect_list;

    if(count_fixed_effect_list) //check the new effect is selected already?
    {
        for(int i = 0;i < ((*fixed_effect_list).length());i++)
        {
            if(QString::number(select_phenotype_index) == (*fixed_effect_list)[i])
            {
                Norepetition_flag = false;
            }
        }
    }
    if(Norepetition_flag)
    {
        *fixed_effect_list << QString::number(select_phenotype_index);
        qDebug()<<endl<<"effect_list:"<<*fixed_effect_list;
    }

}

void remove_item_from_effect_list(QTableView* select_tableview,QStringList* fixed_effect_list)
{
    unsigned int fixed_remove_index = select_tableview->currentIndex().row();
    qDebug()<<endl<<"remove row of effect is"<<fixed_remove_index;
    (*fixed_effect_list).removeAt(fixed_remove_index);
    qDebug()<<endl<<"effect_list:"<<*fixed_effect_list;
}

void change_select_exclude_Button(unsigned int flag,bool elected_flag,QPushButton* select_Button,QPushButton* exclude_Button)
{
    if(elected_flag)
    {
        select_Button->setEnabled(false);
        exclude_Button->setEnabled(false);
    }
    else if(flag)
    {
        select_Button->setEnabled(true);
        exclude_Button->setEnabled(false);
    }
    else
    {
        select_Button->setEnabled(false);
        exclude_Button->setEnabled(true);
    }
}