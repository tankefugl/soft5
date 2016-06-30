#include <QtCore>
#include <gtest/gtest.h>
#include <Soft>
#include <collection.h>
#include <chemkinReader.h>
#include "simple.hxx"
#include "financial.hxx"
#include "physics.hxx"
#include "chemkin_reaction.hxx"
#include "reference.hxx"
#include "file.hxx"

TEST(codegen, dummy)
{
  ASSERT_TRUE(!false);
}

TEST(codegen, construct)
{
  using namespace soft;
  Simple *simple = new Simple (3,2,3,0); 
  ASSERT_TRUE(nullptr != simple);

  auto dims = simple->dimensions();
  ASSERT_STREQ(dims[0].c_str(), "NI");
  ASSERT_STREQ(dims[1].c_str(), "NJ");
  ASSERT_STREQ(dims[2].c_str(), "NK");
  ASSERT_STREQ(dims[3].c_str(), "NL");

  delete simple;
}

TEST(codegen, collectionAdd)
{
  using namespace soft;
  Simple *simple = new Simple (3,2,3,0); 
  Collection collection;
  collection.attachEntity("simple1", simple);
  auto instance = collection.findInstance("simple1");
  ASSERT_EQ(instance, simple);
}

TEST(codegen, storeSimple)
{
  using namespace soft;
  soft::Storage storage("mongo", "mongodb://localhost", "db=codegentest;coll=coll");
  Simple simple(0,0,0,0);
  storage.save(&simple);
}

TEST(codegen, chemkinTest)
{
  using namespace soft;
  const std::string chemfile(SOURCE_DIR  "/SurfaceChemkin.inp");
  const std::string thermfile(SOURCE_DIR "/thermo.dat");
  IO::ChemkinReader chemkinReader(chemfile, thermfile);
  chemkinReader.read();
  auto reactions = chemkinReader.reactions();
  StdDoubleArray A; // preexponential factor
  StdDoubleArray b; 
  StdDoubleArray Ea; 

  StdUInt nreactants = 0;
  StdUInt nproducts = 0;
  StdUInt ntroe = 0;
  StdUInt nenhancement_factors = 0;
  StdUInt nplog = 0;

  auto storage = new soft::Storage("mongo2", "mongodb://localhost", "db=portotest;coll=coll");
  
  int ridx = 0;
  for (auto reaction: reactions) {
    
    auto arrhenius = reaction.getArrhenius();
    auto reactants = reaction.getReactants();
    auto products = reaction.getProducts();
    auto low = reaction.getLOW();
    auto troe = reaction.getTROE();

    Chemkin_reaction chemkinReaction(reactants.size(), 
				     products.size(), 
				     troe.size(), 
				     0 /* nenhancement_factors */,
				     0 /* nplog */);
  
    for(auto reactant: reactants) {
      chemkinReaction.reactants.push_back(reactant.first);
    }
    for(auto product: products) {
      chemkinReaction.products.push_back(product.first);
    }    
    
    chemkinReaction.third_body = reaction.hasThirdBody();
    chemkinReaction.A = arrhenius.A;
    chemkinReaction.b = arrhenius.n;
    chemkinReaction.Ea = arrhenius.E;
    std::cout << chemkinReaction.A << std::endl;

    if (reaction.hasLOW()) {
      ASSERT_EQ(low.size(), 3);      
      chemkinReaction.A_low = low[0];
      chemkinReaction.b_low = low[1];
      chemkinReaction.Ea_low = low[2];
    } else {
      ASSERT_EQ(low.size(), 0);
    }

    for(auto tro: troe) {
      chemkinReaction.troe.push_back(tro);
    }
    ASSERT_EQ(chemkinReaction.A, arrhenius.A);
    ASSERT_EQ(chemkinReaction.b, arrhenius.n);
    ASSERT_EQ(chemkinReaction.Ea, arrhenius.E);
    ridx++;

    storage->save(&chemkinReaction);
  }      
}

static soft::StdBlob toStdBlob(QByteArray const &bytes)
{
  soft::StdBlob blob(bytes.size());
  std::memcpy(blob.data(), bytes.constData(), bytes.size());
  return blob;
}

TEST(codegen, reference)
{
  soft::Reference reference;
  QFileInfo info ("/tmp/thermo-edited.dat");
  reference.uri = "file://" + info.absoluteFilePath().toStdString();
  reference.created = info.created().toString("dd-mm-yyyy").toStdString();
  reference.owner = info.owner().toStdString();
  reference.lastModified = info.lastModified().toString("dd-mm-yyyy").toStdString();  

  QFile file(info.absoluteFilePath());
  if (!file.open(QIODevice::ReadOnly)) {
    FAIL();
  }
  QByteArray buffer = file.readAll();
  QCryptographicHash hash(QCryptographicHash::Sha1);
  hash.addData(buffer.data(), buffer.length()); 
  reference.sha1 = toStdBlob(hash.result());
  soft::Storage storage("mongo2", "mongodb://localhost", "db=codegentest;coll=reference");
  storage.save(&reference);
}

TEST(codegen, file)
{
  soft::File file;
  QFileInfo info ("/tmp/thermo-edited.dat");
  QFile data(info.absoluteFilePath());
  if (!data.open(QIODevice::ReadOnly)) {
    FAIL();
  }
  file.filename = info.fileName().toStdString();
  file.suffix = info.suffix().toStdString();
  file.size = info.size();
  auto buffer = data.readAll();
  file.data = toStdBlob(buffer);
  
  data.close();
  soft::Storage storage("mongo2", "mongodb://localhost", "db=codegentest;coll=filetest");
  storage.save(&file);

  soft::File filecopy(file.id());
  storage.load(&filecopy);

  ASSERT_TRUE(filecopy.filename == file.filename);
  ASSERT_TRUE(filecopy.suffix == file.suffix);
  QTextStream(stdout) << filecopy.data.size() << " -> " << file.data.size() << endl;
  ASSERT_TRUE(filecopy.data.size() == file.data.size());
  QFile dataCopy(info.absoluteFilePath() + ".copy");
  if (!dataCopy.open(QIODevice::WriteOnly |QIODevice::Truncate)) {
    FAIL();
  }

  dataCopy.write((const char*)filecopy.data.data(), (quint64)filecopy.data.size());
}

