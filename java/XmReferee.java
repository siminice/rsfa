import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


public class XmReferee {
  static String mnem = "";
  static String index = "-1";
  static Map<String, String> codes = new HashMap();
  static SimpleDateFormat hudf = new SimpleDateFormat("MMM dd, yyyy");
  static SimpleDateFormat isof = new SimpleDateFormat("yyyy-MM-dd");

  public static void main(String args[]) {
    File input = new File(args[0]);
    mnem = args[1];
    Document doc = null;
    try {
      codes = loadCountryCodes("/var/www/html/country-xm.dat");
    } catch (Exception e) {
      System.err.println("Error: country codes not loaded." + e.getMessage());
    }
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://transfermarkt.us");
    } catch (IOException e) {
      e.printStackTrace();
    }
    extractBio(doc);
  }

  private static void extractBio(Document doc) {
    Element emain = doc.selectFirst(".dataMain");
    Element econt = doc.selectFirst(".dataContent");
    Element edn = emain.selectFirst(".dataName");

    Element esd = doc.selectFirst(".spielerdaten");
    Element tab = esd.selectFirst("table");
    Elements rows = tab.select("tr");
    Element efull = findRow(rows);
    String full = efull!=null ? efull.childNode(0).toString() : "";

    Element h1 = edn.selectFirst("h1");
    int nc = h1.childNodeSize();
    String name  = " ";
    if (nc > 1) {
      Node efn = h1.childNode(0);
      Node eln = h1.childNode(1);
      String fn = efn.toString().replaceAll("\"", "").trim();
      String ln = eln.childNode(0).toString().trim();
      name = fn + " " + ln;
    } else {
      name = h1.childNode(0).childNode(0).toString().trim();
    }
    if (full.length()==0) full = name;

    Element edb = econt.selectFirst(".dataBottom");
    Element edd = edb.selectFirst(".dataDaten");
    Elements props = edd.select("span[itemprop]");

    Element edob = selectProp(props, "birthDate");
    String dob = edob!=null ? edob.childNode(0).toString().split("\\(")[0].trim() : " ";
    String isoDob = toIso(dob);

    Element epob = selectProp(props, "birthPlace");
    String pob = " ";
    String lcob = " ";
    if (epob!=null) {
      Node npob = epob.childNode(0);
      String spob = npob.toString();
      if (npob.hasAttr("title")) spob = npob.attr("title");
      pob = spob.replaceAll(",", " - ");
      Element ep = epob.parent();
      Element ecob = ep.selectFirst("img");
      lcob = ecob!=null ? ecob.attr("title") : " ";
    }

    Element enat = selectProp(props, "nationality");
    String lnat = enat!=null? enat.childNode(0).toString() : " ";

    String nat = codes.values().contains(lnat) ? lnat : codes.getOrDefault(lnat, " ");
    String cob = codes.values().contains(lcob) ? lcob : codes.getOrDefault(lcob, " ");

    System.out.println(String.format("%s,%s,%s,%s,%s,%s,%s,%s",
        mnem, name, full, isoDob, nat, pob, cob, index));
  }

  private static Element findRow(Elements rows) {
    for (Element r : rows) {
      Element th = r.selectFirst("th");
      Element td = r.selectFirst("td");
      if (th!=null && th.childNodeSize() > 0) {
        String title = th.childNode(0).toString();
        if (title.contains("Full name")) return td;
        if (title.contains("Name in home country")) return td;
      }
    }
    return null;
  }

  private static Element selectProp(Elements props, String t) {
    for (Element e : props) {
      if (e.attr("itemprop").equals(t)) return e;
    }
    return null;
  }

  private static String toIso(String dob) {
    if (dob==null) return "0000-00-00";

    try {
      Date d = hudf.parse(dob);
      return isof.format(d);

    } catch (ParseException e) {
      return "0000-00-00";
    }
  }


  private static Map<String, String> loadCountryCodes(String filename) throws IOException {
    Charset charset = Charset.forName("UTF-8");
    List<String> lines = Files.readAllLines(Paths.get(filename), charset);
    Map<String, String> codes = new HashMap();
    for (String s : lines) {
      String[] parts = s.split(",");
      if (parts.length > 1) {
        codes.put(parts[1], parts[0]);
      }
    }
    return codes;
  }
}

