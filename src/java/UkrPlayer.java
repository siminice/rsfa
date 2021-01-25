import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

import com.ibm.icu.text.Transliterator;

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


public class UkrPlayer {
  static String mnem = "";
  static String index = "-1";
  static Map<String, String> codes = new HashMap();
  static SimpleDateFormat hudf = new SimpleDateFormat("dd.MM.yyyy");
  static SimpleDateFormat isof = new SimpleDateFormat("yyyy-MM-dd");
  static Transliterator xlit = Transliterator.getInstance("Cyrillic-Latin");

  public static void main(String args[]) {
    File input = new File(args[0]);
    mnem = args[1];
    Document doc = null;
    try {
      codes = loadCountryCodes("/var/www/html/country-ru.dat");
    } catch (Exception e) {
      System.err.println("Error: country codes not loaded." + e.getMessage());
    }
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://footballfacts.ru");
    } catch (IOException e) {
      e.printStackTrace();
    }
    Element content = doc.selectFirst(".viewitemlayout");
    extractBio(content);
  }

  private static void extractBio(Element content) {
    Elements enames = content.select(".personname");
    String lastrn = "";
    String firsrn = "";
    if (enames.size() > 0) {
      Element cyrname = enames.get(0);
      Element elastr = cyrname.selectFirst(".sname");
      Element efirsr = cyrname.selectFirst(".fname");
      lastrn = elastr.childNode(0).toString().trim();
      firsrn = efirsr.childNode(0).toString().trim();
    }

    String lastln = lastrn;
    String firsln = firsrn;
    Element latname = enames.get(0);
    if (enames.size() > 1) {
      latname = enames.get(1);
      if (enames.size() > 2) {
        for (Element ename : enames) {
          Element pname = ename.parent().selectFirst(":contains(Латиница)");
          if (pname == null) continue;
          else latname = pname;
        }
      }
      Element elastl = latname.selectFirst(".sname");
      Element efirsl = latname.selectFirst(".fname");
      lastln = elastl != null ? elastl.childNode(0).toString().trim() : "";
      firsln = efirsl != null ? efirsl.childNode(0).toString().trim() : "";
    }

    Element edob = content.selectFirst(".date");
    String dob = edob != null ? edob.childNode(0).toString() : "0000-00-00";
    String isoDob = toIso(dob);

    Element epob = content.selectFirst("b:contains(Родился)");
    String pob = " ";
    String lcob = "UKR";
    if (epob != null) {
      epob = epob.parent();
      Elements places = epob.select("a[href]");
      Element ecity = places.size() > 1 ? places.get(1) : null;
      pob = ecity != null && ecity.childNodeSize() > 0 ?
          ecity.childNode(0).toString().trim().replaceAll("г. ", "") : " ";
      lcob = places.size() > 3 && places.get(3).childNodeSize() > 0 ?
          places.get(3).childNode(0).toString().trim() : "UKR";
    }

    Element enat = content.selectFirst("b:contains(Гражданство)");
    String lnat = "UKR";
    if (enat != null) {
      enat = enat.parent();
      Element ectty = enat.selectFirst("a[rel]");
      lnat = ectty!=null && ectty.childNodeSize() > 0 ? ectty.childNode(0).toString().trim() : "UKR";
    }

    String nat = codes.values().contains(lnat) ? lnat : codes.getOrDefault(lnat, " ");
    String cob = codes.values().contains(lcob) ? lcob : codes.getOrDefault(lcob, " ");

    String rname = firsrn + " " + lastrn;
    String lname = firsln + " " + lastln;
    String xname = xlit.transliterate(lname);
    String exname = eliminateMiddle(xname);
    String fxname = xform(exname);
    System.out.println(String.format("%s,%s,%s,%s,%s,%s,%s,%s",
        mnem, fxname, rname, isoDob, nat, pob, cob, index));
  }


  private static String eliminateMiddle(String s) {
    String parts[] = s.split(" ");
    if (parts.length==3) return parts[0] + " " + parts[2];
    return s;
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

  private static boolean isDate(String s) {
    SimpleDateFormat hudf = new SimpleDateFormat("yyyy.MM.dd");
    try {
      hudf.parse(s);
    } catch (ParseException e) {
      return false;
    }
     return true;
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

  private static String xform(String s) {
    s.replaceAll("j", "y");
    return s;
  }
}

