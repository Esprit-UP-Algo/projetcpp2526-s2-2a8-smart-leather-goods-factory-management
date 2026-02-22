#include "production.h"
#include <algorithm>

ProductionData::ProductionData() {}

ProductionData::ProductionData(const QString &id, const QString &reference, const QString &produit,
                               const QString &quantite, const QString &statut,
                               const QDate &dateDebut, const QDate &dateFin,
                               const QString &responsable, const QString &priorite)
    : id(id), reference(reference), produit(produit), quantite(quantite), statut(statut),
      dateDebut(dateDebut), dateFin(dateFin), responsable(responsable), priorite(priorite) {}

// Internal helper
template<typename Pred>
static QList<ProductionData> filtrer(const QList<ProductionData> &l, Pred p)
{ QList<ProductionData> r; std::copy_if(l.begin(),l.end(),std::back_inserter(r),p); return r; }

// Filtering
QList<ProductionData> ProductionData::rechercherParDate(const QList<ProductionData> &l,const QDate &d1,const QDate &d2)
{ QList<ProductionData> r; for(const auto &p:l) if(p.dateDebut>=d1&&p.dateDebut<=d2) r.append(p); return r; }

QList<ProductionData> ProductionData::rechercherParStatut(const QList<ProductionData> &l,const QString &s)
{ return filtrer(l,[&](const ProductionData &p){return p.statut==s;}); }

QList<ProductionData> ProductionData::rechercherParPriorite(const QList<ProductionData> &l,const QString &p)
{ return filtrer(l,[&](const ProductionData &x){return x.priorite==p;}); }

QList<ProductionData> ProductionData::rechercherParResponsable(const QList<ProductionData> &l,const QString &r)
{ return filtrer(l,[&](const ProductionData &x){return x.responsable==r;}); }

// Urgency / delay
QList<ProductionData> ProductionData::obtenirProductionsUrgentes(const QList<ProductionData> &l)
{ return filtrer(l,[](const ProductionData &p){return (p.priorite=="Haute"||p.priorite=="Urgent")&&p.statut!="Terminé";}); }

QList<ProductionData> ProductionData::obtenirProductionsEnRetard(const QList<ProductionData> &l)
{ return filtrer(l,[](const ProductionData &p){return p.estEnRetard();}); }

QList<ProductionData> ProductionData::obtenirProductionsARisque(const QList<ProductionData> &l,int j)
{ return filtrer(l,[j](const ProductionData &p){return p.estARisque(j);}); }

int ProductionData::compterProductionsParStatut(const QList<ProductionData> &l,const QString &s)
{ return std::count_if(l.begin(),l.end(),[&](const ProductionData &p){return p.statut==s;}); }

// Analytics
int ProductionData::calculerQuantiteTotale(const QList<ProductionData> &l,const QString &s)
{ int t=0; for(const auto &p:l) if(s.isEmpty()||p.statut==s) t+=p.quantite.toInt(); return t; }

QMap<QString,int> ProductionData::statistiquesParProduit(const QList<ProductionData> &l)
{ QMap<QString,int> m; for(const auto &p:l) m[p.produit]+=p.quantite.toInt(); return m; }

QMap<QString,int> ProductionData::statistiquesParStatut(const QList<ProductionData> &l)
{ QMap<QString,int> m; for(const auto &p:l) m[p.statut]++; return m; }

QMap<QString,int> ProductionData::statistiquesParResponsable(const QList<ProductionData> &l)
{ QMap<QString,int> m; for(const auto &p:l) m[p.responsable]+=p.quantite.toInt(); return m; }

double ProductionData::calculerTauxAchevementGlobal(const QList<ProductionData> &l)
{ return l.isEmpty()?0.0:100.0*compterProductionsParStatut(l,"Terminé")/l.size(); }

QMap<QString,double> ProductionData::calculerTauxAchevementParProduit(const QList<ProductionData> &l)
{
    QMap<QString,int> tot,done;
    for(const auto &p:l){tot[p.produit]++;if(p.statut=="Terminé")done[p.produit]++;}
    QMap<QString,double> t;
    for(auto it=tot.begin();it!=tot.end();++it) t[it.key()]=100.0*done.value(it.key(),0)/it.value();
    return t;
}

// Sorting
void ProductionData::trierParPriorite(QList<ProductionData> &l,bool dec)
{ std::sort(l.begin(),l.end(),[dec](const ProductionData &a,const ProductionData &b){int pa=prioriteToInt(a.priorite),pb=prioriteToInt(b.priorite);return dec?pa>pb:pa<pb;}); }

void ProductionData::trierParQuantite(QList<ProductionData> &l,bool dec)
{ std::sort(l.begin(),l.end(),[dec](const ProductionData &a,const ProductionData &b){return dec?a.quantite.toInt()>b.quantite.toInt():a.quantite.toInt()<b.quantite.toInt();}); }

void ProductionData::trierParDateDebut(QList<ProductionData> &l,bool dec)
{ std::sort(l.begin(),l.end(),[dec](const ProductionData &a,const ProductionData &b){return dec?a.dateDebut>b.dateDebut:a.dateDebut<b.dateDebut;}); }

void ProductionData::trierParDateFin(QList<ProductionData> &l,bool dec)
{ std::sort(l.begin(),l.end(),[dec](const ProductionData &a,const ProductionData &b){return dec?a.dateFin>b.dateFin:a.dateFin<b.dateFin;}); }

// QR Code
QString ProductionData::genererQRCodeData() const
{ return QString("PROD:%1|REF:%2|STATUT:%3|DATE:%4").arg(id,reference,statut,dateFin.toString("yyyy-MM-dd")); }

QString ProductionData::genererQRCodeComplet() const
{ return QString("ID:%1|REF:%2|PRODUIT:%3|QTE:%4|STATUT:%5|DEBUT:%6|FIN:%7|RESP:%8|PRIO:%9")
         .arg(id,reference,produit,quantite,statut,dateDebut.toString("yyyy-MM-dd"),dateFin.toString("yyyy-MM-dd"),responsable,priorite); }

// Delay checks
bool ProductionData::estEnRetard() const { return QDate::currentDate()>dateFin&&statut!="Terminé"; }
bool ProductionData::estARisque(int j) const { if(statut=="Terminé")return false; int d=joursAvantEcheance(); return d>=0&&d<=j; }
int  ProductionData::joursAvantEcheance() const { return QDate::currentDate().daysTo(dateFin); }

QString ProductionData::getAlerteRetard() const
{
    if(statut=="Terminé") return "✓ Production terminée";
    int j=joursAvantEcheance();
    if(j<0)  return QString("⚠ RETARD : %1 jour(s)").arg(-j);
    if(j==0) return "◉ URGENT : Échéance aujourd'hui !";
    if(j<=3) return QString("◉ ATTENTION : %1 jour(s) restant(s)").arg(j);
    if(j<=7) return QString("◷ À surveiller : %1 jour(s) restant(s)").arg(j);
    return QString("✓ Dans les délais : %1 jour(s) restant(s)").arg(j);
}

QString ProductionData::getNiveauUrgence() const
{
    if(statut=="Terminé") return "Terminé";
    int j=joursAvantEcheance();
    if(j<0)return"Retard"; if(j==0)return"Urgent"; if(j<=3)return"Critique"; if(j<=7)return"Attention";
    return "Normal";
}

// Validation
bool ProductionData::estValide() const
{
    return !id.isEmpty()&&!reference.isEmpty()&&!produit.isEmpty()&&quantite.toInt()>0
        &&!statut.isEmpty()&&dateDebut.isValid()&&dateFin.isValid()&&dateDebut<=dateFin
        &&!responsable.isEmpty()&&!priorite.isEmpty();
}

QStringList ProductionData::obtenirProblemes() const
{
    QStringList p;
    if(id.isEmpty())        p<<"ID manquant";
    if(reference.isEmpty()) p<<"Référence manquante";
    if(produit.isEmpty())   p<<"Produit manquant";
    if(quantite.toInt()<=0) p<<"Quantité invalide";
    if(statut.isEmpty())    p<<"Statut manquant";
    if(!dateDebut.isValid())p<<"Date début invalide";
    if(!dateFin.isValid())  p<<"Date fin invalide";
    if(dateDebut.isValid()&&dateFin.isValid()&&dateDebut>dateFin) p<<"Date début > date fin";
    if(responsable.isEmpty())p<<"Responsable manquant";
    if(priorite.isEmpty())  p<<"Priorité manquante";
    return p;
}

int ProductionData::prioriteToInt(const QString &p)
{
    if(p=="Haute"||p=="Urgent") return 3;
    if(p=="Moyenne"||p=="Normal") return 2;
    if(p=="Basse"||p=="Faible") return 1;
    return 0;
}